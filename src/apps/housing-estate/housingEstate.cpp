#include "housingEstate.h"
#include "GLFW/glfw3.h"
#include "core/mesh.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <memory>

HousingEstate::HousingEstate() {}
HousingEstate::~HousingEstate() {}

void HousingEstate::init_app() {
  std::string resourceDir = "src/apps/housing-estate/res/";
  shader_ = new Shader(resourceDir + "shaders/basic.vert",
                       resourceDir + "shaders/basic.frag");
  camera_ = new Camera(Camera::Perspective(glm::vec3(0.0f, 70.0f, 0.0f),
                                           glm::radians(45.0f),
                                           1920.0f / 1080.0f, 0.1f, 10000.0f));

  // skybox
  std::vector<std::string> faces = {"right.jpg",  "left.jpg",  "top.jpg",
                                    "bottom.jpg", "front.jpg", "back.jpg"};
  skybox_ = new SkyBox("src/apps/housing-estate/res/textures/skybox", faces,
                       "src/apps/housing-estate/res/shaders/skybox.vert",
                       "src/apps/housing-estate/res/shaders/skybox.frag");
  //

  createScene(125);

  GLFWwindow *glfwWin = window->getWindow();
  glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
}

void HousingEstate::input() {
  GLFWwindow *glfwWin = window->getWindow();
  bool spaceCurrentlyPressed =
      (glfwGetKey(glfwWin, GLFW_KEY_SPACE) == GLFW_PRESS);
  if (spaceCurrentlyPressed && !tabPressedLastFrame_) {
    mouseControlEnabled_ = !mouseControlEnabled_;
    if (mouseControlEnabled_) {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      firstMouse = true;
    } else {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
  tabPressedLastFrame_ = spaceCurrentlyPressed;

  if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::FORWARD, deltaTime * cameraSpeed);
  if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::BACKWARD, deltaTime * cameraSpeed);
  if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::RIGHT, deltaTime * cameraSpeed);
  if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::LEFT, deltaTime * cameraSpeed);

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

void HousingEstate::update() {
  if (rootNode_) {
    rootNode_->updateTransform(glm::mat4(1.0f));
  }

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
}

void HousingEstate::render() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view = camera_->getViewMatrix();
  glm::mat4 projection = camera_->getProjection();

  skybox_->draw(view, projection);

  shader_->use();
  shader_->setUniform("view", view);
  shader_->setUniform("projection", projection);

  shader_->setUniform("isInstanced", false);
  if (rootNode_) {
    rootNode_->draw(*shader_);
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

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void HousingEstate::render_gui() {
  ImGui::Begin("Performance");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void HousingEstate::createScene(int houseCount) {
  rootNode_ = std::make_shared<GraphNode>();

  std::string textureDir = "src/apps/housing-estate/res/textures";
  Model *planeModel = generatePlaneModel(1000.0f, textureDir, "grass.jpg");
  Model *wallModel = generateCubeModel(5.0f, textureDir);
  Model *roofModel = generatePyramidModel(6.0f, textureDir, "roof.jpg");

  for (auto &mesh : wallModel->getMeshes())
    mesh.setupInstancing(houseCount * houseCount);
  for (auto &mesh : roofModel->getMeshes())
    mesh.setupInstancing(houseCount * houseCount);

  auto planeNode = std::make_shared<GraphNode>(planeModel);
  rootNode_->addChild(planeNode);

  float spacing = 15.0f;
  float offset = (houseCount - 1) * spacing / 2.0f;

  for (int i = 0; i < houseCount; i++) {
    for (int j = 0; j < houseCount; j++) {
      auto houseRoot = std::make_shared<GraphNode>();
      float posX = i * spacing - offset;
      float posZ = j * spacing - offset;
      houseRoot->getTransform().setPosition(glm::vec3(posX, 2.5f, posZ));

      auto walls = std::make_shared<GraphNode>(wallModel);
      // walls->setIsInstancedRendering(false);
      walls->setIsInstancedRendering(true);
      houseRoot->addChild(walls);

      auto roof = std::make_shared<GraphNode>(roofModel);
      // roof->setIsInstancedRendering(false);
      roof->setIsInstancedRendering(true);
      roof->getTransform().setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
      houseRoot->addChild(roof);

      rootNode_->addChild(houseRoot);
      houses_.push_back({houseRoot, walls, roof});
    }
  }
}

Model *HousingEstate::generatePlaneModel(float size, std::string textureDir,
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

Model *HousingEstate::generateCubeModel(float size, std::string textureDir) {
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
          "red_brick_window.jpg");

  return model;
}

Model *HousingEstate::generatePyramidModel(float size, std::string textureDir,
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
