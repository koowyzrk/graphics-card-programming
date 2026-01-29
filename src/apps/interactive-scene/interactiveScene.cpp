#include "interactiveScene.h"
#include "GLFW/glfw3.h"
#include "core/light.h"
#include "core/mesh.h"
#include "core/transform.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include <memory>

InteractiveScene::InteractiveScene() {}
InteractiveScene::~InteractiveScene() {}

void InteractiveScene::init_app() {
  std::string resourceDir = "src/apps/interactive-scene/res/";
  shader_ = new Shader(resourceDir + "shaders/basic.vert",
                       resourceDir + "shaders/basic.frag");

  reflectShader_ = new Shader(resourceDir + "shaders/ref.vert",
                              resourceDir + "shaders/reflect.frag");

  refractionShader_ = new Shader(resourceDir + "shaders/ref.vert",
                                 resourceDir + "shaders/refract.frag");

  camera_ = new Camera(Camera::Perspective(glm::vec3(0.0f, 100.0f, 400.0f),
                                           glm::radians(45.0f),
                                           1920.0f / 1080.0f, 0.1f, 10000.0f));
  camera_->setYaw(10.0f);
  camera_->setPitch(-60.0f);

  // skybox
  std::vector<std::string> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                                    "bottom.jpg", "front.jpg", "back.jpg"};
  skybox_ = new SkyBox("src/apps/interactive-scene/res/textures/skybox", faces,
                       "src/apps/interactive-scene/res/shaders/skybox.vert",
                       "src/apps/interactive-scene/res/shaders/skybox.frag");
  //

  // lights
  directional_ = new Light("directional", LightType::DIRECTIONAL);
  spotlight1_ = new Light("spotlight_1", LightType::SPOT);
  spotlight2_ = new Light("spotlight_2", LightType::SPOT);
  point_ = new Light("point", LightType::POINT);

  std::string textureDir = "src/apps/interactive-scene/res/textures";
  lightVisualModel = generateCubeModel(1.0f, textureDir);
  //

  createScene(20);

  GLFWwindow *glfwWin = window->getWindow();
  glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
}

void InteractiveScene::input() {
  GLFWwindow *glfwWin = window->getWindow();

  static bool gPressedLastFrame = false;
  bool gPressed = (glfwGetKey(glfwWin, GLFW_KEY_G) == GLFW_PRESS);
  if (gPressed && !gPressedLastFrame) {
    isDriving = !isDriving;
    firstMouse = true;
    mouseControlEnabled_ = true;
    glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  gPressedLastFrame = gPressed;

  static bool spacePressedLastFrame = false;
  bool spacePressed = (glfwGetKey(glfwWin, GLFW_KEY_SPACE) == GLFW_PRESS);
  if (spacePressed && !spacePressedLastFrame) {
    mouseControlEnabled_ = !mouseControlEnabled_;
    glfwSetInputMode(glfwWin, GLFW_CURSOR,
                     mouseControlEnabled_ ? GLFW_CURSOR_DISABLED
                                          : GLFW_CURSOR_NORMAL);
    firstMouse = true;
  }
  spacePressedLastFrame = spacePressed;

  if (isDriving) {
    float acceleration = 0.0f;
    if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
      acceleration = 1.0f;
    if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
      acceleration = -1.0f;

    const float accelPower = 600.0f;
    const float steeringSpeed = 3.5f;
    const float drag = 0.985f;

    car.speed += acceleration * accelPower * deltaTime;
    car.speed *= drag;

    if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
      car.steeringAngle += steeringSpeed * deltaTime;
    if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
      car.steeringAngle -= steeringSpeed * deltaTime;

    car.steeringAngle = glm::clamp(car.steeringAngle, glm::radians(-30.0f),
                                   glm::radians(30.0f));
  } else {
    if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
      camera_->processKeyboard(CameraMovement::FORWARD,
                               deltaTime * cameraSpeed);
    if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
      camera_->processKeyboard(CameraMovement::BACKWARD,
                               deltaTime * cameraSpeed);
    if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
      camera_->processKeyboard(CameraMovement::RIGHT, deltaTime * cameraSpeed);
    if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
      camera_->processKeyboard(CameraMovement::LEFT, deltaTime * cameraSpeed);
  }

  if (mouseControlEnabled_) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwWin, &mouseX, &mouseY);

    if (firstMouse) {
      lastX = mouseX;
      lastY = mouseY;
      firstMouse = false;
    }

    float xOffset = mouseX - lastX;
    float yOffset = lastY - mouseY;
    lastX = mouseX;
    lastY = mouseY;

    camera_->processMouseMovement(xOffset, yOffset);
  }
}

void InteractiveScene::update() {
  // lights
  float time = (float)glfwGetTime();
  for (auto &l : lights) {
    if (l->getLight().getName() == "point") {
      l->getTransform().setPosition(
          glm::vec3(sin(time) * 50.0f, 30.0f, cos(time) * 50.0f));
    }
    if (lightsRoot) {
      lightsRoot->updateTransform(rootNode_->getGlobalTransform());
    }
  }
  //

  if (car.root) {
    Transform &trans = car.root->getTransform();

    car.yaw += (car.steeringAngle * car.speed / 100.0f) * deltaTime;
    trans.setRotation(glm::vec3(0.0f, car.yaw, 0.0f));

    glm::vec3 forwardDir = glm::vec3(sin(car.yaw), 0.0f, cos(car.yaw));
    glm::vec3 pos = trans.getPosition();
    pos += forwardDir * car.speed * deltaTime;
    trans.setPosition(pos);

    // toczenie kół
    car.wheelAngle += car.speed * deltaTime * 0.1f;

    GLFWwindow *glfwWin = window->getWindow();
    float steerAmount = 0.0f;
    if (isDriving) {
      if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
        steerAmount = glm::radians(30.0f);
      if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
        steerAmount = glm::radians(-30.0f);
    }

    car.left_wheel->getTransform().setRotation(
        glm::vec3(car.wheelAngle, car.steeringAngle, 0.0f));
    car.right_wheel->getTransform().setRotation(
        glm::vec3(car.wheelAngle, car.steeringAngle, 0.0f));

    car.left_back_wheel->getTransform().setRotation(
        glm::vec3(car.wheelAngle, 0.0f, 0.0f));
    car.right_back_wheel->getTransform().setRotation(
        glm::vec3(car.wheelAngle, 0.0f, 0.0f));

    float targetAngle = car.doorOpen ? glm::radians(45.0f) : 0.0f;
    car.doorOpenAngle += (targetAngle - car.doorOpenAngle) * deltaTime * 5.0f;
    if (car.left_doors)
      car.left_doors->getTransform().setRotation(
          glm::vec3(0, 0, car.doorOpenAngle));
  }

  if (isDriving) {
    updateCameraFollow();
  }

  if (rootNode_) {
    rootNode_->updateTransform(glm::mat4(1.0f));
  }

  if (bufferUpdate) {
    std::vector<glm::mat4> wallMatrices;
    std::vector<glm::mat4> roofMatrices;
    wallMatrices.reserve(houses_.size());
    roofMatrices.reserve(houses_.size());

    for (const auto &house : houses_) {
      wallMatrices.push_back(house.wall->getGlobalTransform());
      roofMatrices.push_back(house.roof->getGlobalTransform());
    }

    if (!houses_.empty()) {
      Model *wModel = houses_[0].wall->getModel();
      for (auto &mesh : wModel->getMeshes())
        mesh.updateInstanceBuffer(wallMatrices);
      Model *rModel = houses_[0].roof->getModel();
      for (auto &mesh : rModel->getMeshes())
        mesh.updateInstanceBuffer(roofMatrices);
    }
    bufferUpdate = false;
  }
}

void InteractiveScene::render() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view = camera_->getViewMatrix();
  glm::mat4 projection = camera_->getProjection();

  skybox_->draw(view, projection);

  shader_->use();
  shader_->setUniform("view", view);
  shader_->setUniform("projection", projection);
  shader_->setUniform("viewPos", camera_->getPosition());

  // lights
  int activeLights = std::min((int)lights.size(), 10);
  for (int i = 0; i < activeLights; i++) {
    lights[i]->getLight().sendToShader(*shader_, i, lights[i]->getTransform());
  }
  shader_->setUniform("lightCount", activeLights);

  shader_->setUniform("isInstanced", false);
  if (rootNode_) {
    for (auto &child : rootNode_->getChildren()) {
      if (child == reflectiveNode_ || child == refractionNode_) {
        continue;
      }
      child->draw(*shader_);
    }
  }

  if (!houses_.empty()) {
    shader_->setUniform("isInstanced", true);
    unsigned int count = (unsigned int)houses_.size();
    Model *wModel = houses_[0].wall->getModel();
    for (auto &mesh : wModel->getMeshes()) {
      mesh.drawInstanced(*shader_, count);
    }
    Model *rModel = houses_[0].roof->getModel();
    for (auto &mesh : rModel->getMeshes()) {
      mesh.drawInstanced(*shader_, count);
    }
  }
  shader_->setUniform("isInstanced", false);

  if (car.glass) {
    reflectShader_->use();
    reflectShader_->setUniform("view", view);
    reflectShader_->setUniform("projection", projection);
    reflectShader_->setUniform("cameraPos", camera_->getPosition());
    reflectShader_->setUniform("model", car.glass->getGlobalTransform());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->getTextureID());
    reflectShader_->setUniform("skybox", 0);

    car.glass->getModel()->draw(*reflectShader_);
  }

  if (car.spoiler) {
    refractionShader_->use();
    refractionShader_->setUniform("view", view);
    refractionShader_->setUniform("projection", projection);
    refractionShader_->setUniform("cameraPos", camera_->getPosition());
    refractionShader_->setUniform("model", car.spoiler->getGlobalTransform());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->getTextureID());
    refractionShader_->setUniform("skybox", 0);

    car.spoiler->getModel()->draw(*refractionShader_);
  }

  if (reflectiveNode_) {
    reflectShader_->use();
    reflectShader_->setUniform("view", view);
    reflectShader_->setUniform("projection", projection);
    reflectShader_->setUniform("cameraPos", camera_->getPosition());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->getTextureID());
    reflectShader_->setUniform("skybox", 0);

    reflectiveNode_->draw(*reflectShader_);
  }

  if (refractionNode_) {
    refractionShader_->use();
    refractionShader_->setUniform("view", view);
    refractionShader_->setUniform("projection", projection);
    refractionShader_->setUniform("cameraPos", camera_->getPosition());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_->getTextureID());
    refractionShader_->setUniform("skybox", 0);

    refractionNode_->draw(*refractionShader_);
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void InteractiveScene::render_gui() {
  ImGui::Begin("Housing Estate");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  if (ImGui::CollapsingHeader("Whole scene transformation")) {
    if (rootNode_) {
      Transform &rootTrans = rootNode_->getTransform();
      glm::vec3 rootPos = rootTrans.getPosition();
      glm::vec3 rootRot = glm::degrees(rootTrans.getRotation());
      glm::vec3 rootScl = rootTrans.getScale();
      if (ImGui::DragFloat3("Position", &rootPos.x, 0.1f)) {
        rootTrans.setPosition(rootPos);
      }
      if (ImGui::DragFloat3("Rotation", &rootRot.x, 1.0f)) {
        rootTrans.setRotation(glm::radians(rootRot));
      }
      if (ImGui::DragFloat3("Scale", &rootScl.x, 0.05f, 0.01f, 10.0f)) {
        rootTrans.setScale(rootScl);
      }
    }
  }

  if (ImGui::CollapsingHeader("Object Selection")) {
    int maxIdx = static_cast<int>(houses_.size()) - 1;
    ImGui::SliderInt("House Index", &selectedHouseIndex, 0, maxIdx);
    ImGui::RadioButton("Whole House/Walls", (int *)&editHouse, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Roof Only", (int *)&editHouse, 1);
  }

  if (!houses_.empty() && selectedHouseIndex >= 0 &&
      selectedHouseIndex < houses_.size()) {
    ImGui::Separator();
    ImGui::Text("Editing House: %d", selectedHouseIndex);
    std::shared_ptr<GraphNode> targetNode =
        editHouse ? houses_[selectedHouseIndex].roof
                  : houses_[selectedHouseIndex].root;

    Transform &trans = targetNode->getTransform();

    glm::vec3 pos = trans.getPosition();
    glm::vec3 rot = glm::degrees(trans.getRotation());
    glm::vec3 scl = trans.getScale();

    if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
      trans.setPosition(pos);
      bufferUpdate = true;
    }
    if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f)) {
      trans.setRotation(glm::radians(rot));
      bufferUpdate = true;
    }
    if (ImGui::DragFloat3("Scale", &scl.x, 0.05f, 0.01f, 10.0f)) {
      trans.setScale(scl);
      bufferUpdate = true;
    }
  }

  if (ImGui::CollapsingHeader("Lighting System")) {
    for (auto &l : lights) {
      if (ImGui::TreeNode(l->getLight().getName().c_str())) {

        bool enabled = l->getLight().isEnabled();
        if (ImGui::Checkbox("Enabled", &enabled)) {
          l->getLight().setEnabled(enabled);
        }

        glm::vec3 color = l->getLight().getColor();
        if (ImGui::ColorEdit3("Color", &color[0])) {
          l->getLight().setColor(color);
        }

        float intensity = l->getLight().getIntensity();
        if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f)) {
          l->getLight().setIntensity(intensity);
        }

        Transform &trans = l->getTransform();

        if (l->getLight().getType() == LightType::DIRECTIONAL ||
            l->getLight().getType() == LightType::SPOT) {
          glm::vec3 rot = trans.getRotation();
          if (ImGui::DragFloat3("Direction (Rotation)", &rot.x, 1.0f)) {
            trans.setRotation(rot);
          }
        }

        if (l->getLight().getType() == LightType::POINT ||
            l->getLight().getType() == LightType::SPOT) {
          glm::vec3 pos = trans.getPosition();
          if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
            trans.setPosition(pos);
          }
        }

        if (l->getLight().getType() == LightType::SPOT) {
          float cut = l->getLight().getCutOff();
          if (ImGui::SliderFloat("Cutoff Angle", &cut, 0.0f, 45.0f)) {
            l->getLight().setCutOff(cut);
          }
        }

        ImGui::TreePop();
      }
    }
  }

  ImGui::End();
}

void InteractiveScene::createScene(int houseCount) {
  rootNode_ = std::make_shared<GraphNode>();

  std::string textureDir = "src/apps/interactive-scene/res/textures";
  Model *planeModel = generatePlaneModel(1000.0f, textureDir, "grass.jpg");
  Model *wallModel = generateCubeModel(5.0f, textureDir);
  Model *roofModel = generatePyramidModel(6.0f, textureDir, "roof.jpg");

  auto planeNode = std::make_shared<GraphNode>(planeModel);
  rootNode_->addChild(planeNode);

  std::string resourceDir = "src/apps/interactive-scene/res/models";
  // interactice scene other objects
  reflectiveNode_ = std::make_shared<GraphNode>();
  reflectiveNode_->setSkipDraw(true);
  rootNode_->addChild(reflectiveNode_);

  refractionNode_ = std::make_shared<GraphNode>();
  refractionNode_->setSkipDraw(true);
  rootNode_->addChild(refractionNode_);

  cube_ = std::make_unique<Model>(resourceDir + "/cube/cube.gltf");
  auto cubeNode = std::make_shared<GraphNode>(cube_.get());
  cubeNode->getTransform().setPosition(glm::vec3(-100, 100.0f, 0));
  cubeNode->getTransform().setScale(glm::vec3(25.0f, 25.0f, 25.0f));
  reflectiveNode_->addChild(cubeNode);

  character_ = std::make_unique<Model>(resourceDir + "/zelda/zelda.gltf");
  auto characterNode = std::make_shared<GraphNode>(character_.get());
  characterNode->getTransform().setPosition(glm::vec3(100, 100.0f, 0));
  characterNode->getTransform().setScale(glm::vec3(40.0f, 40.0f, 40.0f));
  refractionNode_->addChild(characterNode);
  //

  // car
  car.root = std::make_shared<GraphNode>();
  car.root->getTransform().setPosition(glm::vec3(0, 0.0f, 200));
  car.root->getTransform().setScale(glm::vec3(10.0f));
  rootNode_->addChild(car.root);

  carBody_ = std::make_unique<Model>(resourceDir + "/car/car_body.gltf");
  car.body = std::make_shared<GraphNode>(carBody_.get());
  car.root->addChild(car.body);

  carGlass_ = std::make_unique<Model>(resourceDir + "/car/car_glass.gltf");
  car.glass = std::make_shared<GraphNode>(carGlass_.get());
  car.glass->setSkipDraw(true);
  car.root->addChild(car.glass);

  leftWheel_ = std::make_unique<Model>(resourceDir + "/car/left_wheel.gltf");
  car.left_wheel = std::make_shared<GraphNode>(leftWheel_.get());
  car.left_wheel->getTransform().setPosition(glm::vec3(-1.7f, -0.5f, 1.0f));
  car.root->addChild(car.left_wheel);

  rightWheel_ = std::make_unique<Model>(resourceDir + "/car/right_wheel.gltf");
  car.right_wheel = std::make_shared<GraphNode>(rightWheel_.get());
  car.root->addChild(car.right_wheel);

  leftBackWheel_ =
      std::make_unique<Model>(resourceDir + "/car/left_back_wheel.gltf");
  car.left_back_wheel = std::make_shared<GraphNode>(leftBackWheel_.get());
  car.root->addChild(car.left_back_wheel);

  rightBackWheel_ =
      std::make_unique<Model>(resourceDir + "/car/right_back_wheel.gltf");
  car.right_back_wheel = std::make_shared<GraphNode>(rightBackWheel_.get());
  car.root->addChild(car.right_back_wheel);

  leftDoors_ = std::make_unique<Model>(resourceDir + "/car/left_door.gltf");
  car.left_doors = std::make_shared<GraphNode>(leftDoors_.get());
  car.root->addChild(car.left_doors);

  rightDoors_ = std::make_unique<Model>(resourceDir + "/car/right_door.gltf");
  car.right_doors = std::make_shared<GraphNode>(rightDoors_.get());
  car.root->addChild(car.right_doors);

  carSpoiler_ = std::make_unique<Model>(resourceDir + "/car/spoiler.gltf");
  car.spoiler = std::make_shared<GraphNode>(carSpoiler_.get());
  car.spoiler->setSkipDraw(true);
  car.root->addChild(car.spoiler);
  //

  // lights
  lightsRoot = std::make_shared<GraphNode>();
  rootNode_->addChild(lightsRoot);

  auto lightDirNode =
      std::make_shared<LightNode>(*directional_, lightVisualModel);
  auto lightSpotOneNode =
      std::make_shared<LightNode>(*spotlight1_, lightVisualModel);
  auto lightSpotTwoNode =
      std::make_shared<LightNode>(*spotlight2_, lightVisualModel);
  auto lightPointNode = std::make_shared<LightNode>(*point_, lightVisualModel);

  lights.push_back(lightDirNode);
  lights.back()->getTransform().setRotation(
      glm::radians(glm::vec3(-2800.0f, 0.0f, 0.0f)));
  lights.back()->getTransform().setPosition(glm::vec3(0, 40, 0));

  lights.push_back(lightSpotOneNode);
  lights.back()->getTransform().setPosition(glm::vec3(-20, 10, 0));

  lights.push_back(lightSpotTwoNode);
  lights.back()->getTransform().setPosition(glm::vec3(20, 10, 0));

  lights.push_back(lightPointNode);

  lightsRoot->addChild(lightDirNode);
  lightsRoot->addChild(lightSpotOneNode);
  lightsRoot->addChild(lightSpotTwoNode);
  lightsRoot->addChild(lightPointNode);
  //

  //
  for (auto &mesh : wallModel->getMeshes())
    mesh.setupInstancing(houseCount * houseCount);
  for (auto &mesh : roofModel->getMeshes())
    mesh.setupInstancing(houseCount * houseCount);

  float spacing = 15.0f;
  float offset = (houseCount - 1) * spacing / 2.0f;

  for (int i = 0; i < houseCount; i++) {
    for (int j = 0; j < houseCount; j++) {
      auto houseRoot = std::make_shared<GraphNode>();
      float posX = i * spacing - offset;
      float posZ = j * spacing - offset;
      houseRoot->getTransform().setPosition(glm::vec3(posX, 2.5f, posZ));

      if (i % 2) {
        if (!(j % 2))
          houseRoot->getTransform().setScale(glm::vec3(2.0f, 2.0f, 2.0f));
      }

      if (!(i % 2)) {
        if ((j % 2))
          houseRoot->getTransform().setScale(glm::vec3(2.0f, 2.0f, 2.0f));
      }

      auto walls = std::make_shared<GraphNode>(wallModel);
      // walls->setIsInstancedRendering(false);
      walls->setSkipDraw(true);
      houseRoot->addChild(walls);

      auto roof = std::make_shared<GraphNode>(roofModel);
      // roof->setIsInstancedRendering(false);
      roof->setSkipDraw(true);
      roof->getTransform().setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
      houseRoot->addChild(roof);

      rootNode_->addChild(houseRoot);
      houses_.push_back({houseRoot, walls, roof});
    }
  }
}

void InteractiveScene::updateCameraFollow() {
  Transform &carTrans = car.root->getTransform();
  glm::vec3 carPos = carTrans.getPosition();

  float distance = 150.0f;

  float yaw = camera_->getYaw();
  float pitch = camera_->getPitch();

  glm::vec3 offset;
  offset.x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
  offset.y = distance * sin(glm::radians(pitch));
  offset.z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

  camera_->setPosition(carPos - offset);
}

Model *InteractiveScene::generatePlaneModel(float size, std::string textureDir,
                                            std::string textureFile) {
  Model *model = new Model();

  float uvScale = 100.0f;

  std::vector<Vertex> vertices;
  glm::vec3 up(0.0f, 1.0f, 0.0f);
  glm::vec3 tangent(1.0f, 0.0f, 0.0f);

  vertices.push_back(
      {glm::vec3(-size, 0.0f, -size), up, glm::vec2(0.0f, uvScale), tangent});
  vertices.push_back(
      {glm::vec3(-size, 0.0f, size), up, glm::vec2(0.0f, 0.0f), tangent});
  vertices.push_back(
      {glm::vec3(size, 0.0f, size), up, glm::vec2(uvScale, 0.0f), tangent});
  vertices.push_back(
      {glm::vec3(size, 0.0f, -size), up, glm::vec2(uvScale, uvScale), tangent});

  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

  std::vector<Texture> textures;
  Texture groundTex;

  groundTex.id = model->textureFromFile(textureFile.c_str(), textureDir);
  groundTex.type = "texture_diffuse";
  groundTex.path = textureFile.c_str();
  textures.push_back(groundTex);

  Mesh mesh(vertices, indices, textures);
  model->addMesh(mesh);
  return model;
}

Model *InteractiveScene::generateCubeModel(float size, std::string textureDir) {
  Model *model = new Model();
  float s = size / 2.0f;

  auto addFace = [&](std::vector<glm::vec3> pos, glm::vec3 norm,
                     std::string texFile) {
    std::vector<Vertex> v;
    v.push_back({pos[0], norm, {0, 1}, {1, 0, 0}});
    v.push_back({pos[1], norm, {1, 1}, {1, 0, 0}});
    v.push_back({pos[2], norm, {1, 0}, {1, 0, 0}});
    v.push_back({pos[3], norm, {0, 0}, {1, 0, 0}});

    std::vector<unsigned int> ind = {0, 1, 2, 0, 2, 3};
    std::vector<Texture> textures;
    Texture t;
    t.id = model->textureFromFile(texFile.c_str(), textureDir);
    t.type = "texture_diffuse";
    textures.push_back(t);

    model->addMesh(Mesh(v, ind, textures));
  };

  addFace({{-s, -s, s}, {s, -s, s}, {s, s, s}, {-s, s, s}}, {0, 0, 1},
          "red_brick_door.jpg");
  addFace({{-s, -s, -s}, {-s, s, -s}, {s, s, -s}, {s, -s, -s}}, {0, 0, -1},
          "red_brick.jpg");
  addFace({{-s, -s, -s}, {-s, -s, s}, {-s, s, s}, {-s, s, -s}}, {-1, 0, 0},
          "red_brick_window.jpg");
  addFace({{s, -s, -s}, {s, s, -s}, {s, s, s}, {s, -s, s}}, {1, 0, 0},
          "#include <iostream>red_brick_window.jpg");

  return model;
}

Model *InteractiveScene::generatePyramidModel(float size,
                                              std::string textureDir,
                                              std::string textureFile) {
  Model *model = new Model();
  std::vector<Vertex> vertices;
  float h = size * 0.8f;
  float s = size / 2.0f;
  glm::vec3 peak(0.0f, h, 0.0f);

  auto addFace = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
    vertices.push_back({p1, normal, {0.0f, 0.0f}, {1, 0, 0}});
    vertices.push_back({p2, normal, {1.0f, 0.0f}, {1, 0, 0}});
    vertices.push_back({p3, normal, {0.5f, 1.0f}, {1, 0, 0}});
  };

  addFace({-s, 0, s}, {s, 0, s}, peak);   // Przód
  addFace({s, 0, s}, {s, 0, -s}, peak);   // Prawo
  addFace({s, 0, -s}, {-s, 0, -s}, peak); // Tył
  addFace({-s, 0, -s}, {-s, 0, s}, peak); // Lewo

  std::vector<unsigned int> indices;
  for (unsigned int i = 0; i < vertices.size(); i++)
    indices.push_back(i);

  std::vector<Texture> textures;
  Texture tex;
  tex.id = model->textureFromFile(textureFile.c_str(), textureDir);
  tex.type = "texture_diffuse";
  tex.path = textureFile;
  textures.push_back(tex);

  model->addMesh(Mesh(vertices, indices, textures));
  return model;
}
