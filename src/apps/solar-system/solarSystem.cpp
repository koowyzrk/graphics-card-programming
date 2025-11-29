#include "solarSystem.h"

SolarSystem::SolarSystem() {}
SolarSystem::~SolarSystem() {}

void SolarSystem::init_app() {
  std::string resourceDir = "src/apps/solar-system/res/";
  shader_ = new Shader(resourceDir + "shaders/basic.vert",
                       resourceDir + "shaders/basic.frag");
  camera_ = new Camera(Camera::Perspective(glm::vec3(0.0f, 0.0f, 0.0f),
                                           glm::radians(45.0f),
                                           1920.0f / 1080.0f, 0.1f, 100.0f));

  sunModel_ = std::make_unique<Model>(resourceDir + "models/sun/scene.gltf");

  mercuryModel_ =
      std::make_unique<Model>(resourceDir + "models/mercury/scene.gltf");
  venusModel_ =
      std::make_unique<Model>(resourceDir + "models/venus/scene.gltf");

  earthModel_ =
      std::make_unique<Model>(resourceDir + "models/earth/scene.gltf");
  moonModel_ = std::make_unique<Model>(resourceDir + "models/moon/scene.gltf");

  marsModel_ = std::make_unique<Model>(resourceDir + "models/mars/scene.gltf");

  jupiterModel_ =
      std::make_unique<Model>(resourceDir + "models/jupiter/scene.gltf");
  jupiterMoonOne_ = std::unique_ptr<Model>(generateSphereModel(32, 32));

  saturnModel_ =
      std::make_unique<Model>(resourceDir + "models/saturn/scene.gltf");
  saturnMoonOne_ = std::unique_ptr<Model>(generateSphereModel(32, 32));

  uranModel_ =
      std::make_unique<Model>(resourceDir + "models/uranus/scene.gltf");
  neptuneModel_ =
      std::make_unique<Model>(resourceDir + "models/neptune/scene.gltf");

  orbitModel_ = std::unique_ptr<Model>(generateOrbitModel(64));

  createScene();
  GLFWwindow *glfwWin = window->getWindow();
  glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glEnable(GL_DEPTH_TEST);
}

void SolarSystem::input() {
  GLFWwindow *glfwWin = window->getWindow();
  bool tabCurrentlyPressed = (glfwGetKey(glfwWin, GLFW_KEY_TAB) == GLFW_PRESS);
  if (tabCurrentlyPressed && !tabPressedLastFrame_) {
    mouseControlEnabled_ = !mouseControlEnabled_;
    if (mouseControlEnabled_) {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      firstMouse = true; // Zresetuj flagę, aby uniknąć skoku kamery
    } else {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
  tabPressedLastFrame_ = tabCurrentlyPressed;

  if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::FORWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::BACKWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::RIGHT, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement::LEFT, deltaTime);

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

void SolarSystem::update() {
  float dt = deltaTime * globalSpeedMultiplier_;

  for (auto &body : celestialBodies_) {
    body.currentOrbitAngle += body.orbitSpeed * dt;
    body.currentSelfAngle += body.selfRotationSpeed * dt;
    if (body.orbitRadius > 0.0f) {
      float angleRad = glm::radians(body.currentOrbitAngle);
      float x = glm::cos(angleRad) * body.orbitRadius;
      float z = glm::sin(angleRad) * body.orbitRadius;
      body.node->getTransform().setPosition(glm::vec3(x, 0.0f, z));
    }
    body.node->getTransform().setRotation(
        glm::vec3(0.0f, body.currentSelfAngle, 0.0f));
  }
  if (rootNode_) {
    rootNode_->getTransform().setRotation(
        glm::vec3(0.0f, sceneRotationAngle_, 0.0f));
    rootNode_->updateTransform(glm::mat4(1.0f));
  }
}

void SolarSystem::render() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view = camera_->getViewMatrix();
  glm::mat4 projection = camera_->getProjection();

  shader_->use();
  shader_->setUniform("view", view);
  shader_->setUniform("projection", projection);

  if (wireframeMode_)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // if (rootNode_) {
  //   shader_->setUniform("useTexture", true);
  //   rootNode_->draw(*shader_);
  // }

  for (const auto &body : celestialBodies_) {
    if (!body.node || !body.node->getModel())
      continue;

    shader_->setUniform("model", body.node->getGlobalTransform());
    bool isGeneratedMoon = (body.node->getModel() == jupiterMoonOne_.get() ||
                            body.node->getModel() == saturnMoonOne_.get());
    shader_->setUniform("objectColor", body.orbitColor);
    if (isGeneratedMoon) {
      shader_->setUniform("useTexture", false);
    } else {
      shader_->setUniform("useTexture", true);
    }
    body.node->getModel()->draw(*shader_);
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  drawOrbits();
}

void SolarSystem::render_gui() {
  ImGui::Begin("Controls");

  // wireframe view
  if (ImGui::Checkbox("Tryb Siatki (Wireframe)", &wireframeMode_)) {
    ;
  }
  // spppeeed
  ImGui::Separator();
  ImGui::SliderFloat("Predkosc Symulacji", &globalSpeedMultiplier_, 0.0f, 10.0f,
                     "%.1f x");
  // rotation of solar system
  ImGui::Separator();
  ImGui::SliderFloat("Obrot Układu Slonecznego (Y)", &sceneRotationAngle_, 0.0f,
                     360.0f, "%.1f deg");
  // szzczegolowosc siatki
  if (ImGui::SliderInt("Segmenty Sfery/Orbity", &currentTessellation_, 8,
                       128)) {
    regenerateGeneratedModels();
  }

  ImGui::End();
}

void SolarSystem::createScene() {
  celestialBodies_.clear();
  rootNode_ = std::make_shared<GraphNode>(nullptr);

  // --- SŁOŃCE ---
  auto sunNode = std::make_shared<GraphNode>(sunModel_.get());
  sunNode->getTransform().setScale(glm::vec3(0.2f));
  rootNode_->addChild(sunNode);

  CelestialBody sunData;
  sunData.node = sunNode;
  sunData.orbitRadius = 0.0f;
  sunData.orbitSpeed = 0.0f;
  sunData.selfRotationSpeed = 2.0f;
  sunData.orbitColor = glm::vec3(1.0f, 1.0f, 0.0f);
  celestialBodies_.push_back(sunData);

  // --- MERKURY ---
  auto mercuryNode = std::make_shared<GraphNode>(mercuryModel_.get());
  mercuryNode->getTransform().setScale(glm::vec3(0.08f));
  rootNode_->addChild(mercuryNode);

  CelestialBody mercuryData;
  mercuryData.node = mercuryNode;
  mercuryData.orbitRadius = 3.0f;
  mercuryData.orbitSpeed = 45.0f;
  mercuryData.selfRotationSpeed = 5.0f;
  mercuryData.orbitColor = glm::vec3(0.6f, 0.6f, 0.6f);
  celestialBodies_.push_back(mercuryData);

  // --- WENUS ---
  auto venusNode = std::make_shared<GraphNode>(venusModel_.get());
  venusNode->getTransform().setScale(glm::vec3(0.12f));
  rootNode_->addChild(venusNode);

  CelestialBody venusData;
  venusData.node = venusNode;
  venusData.orbitRadius = 4.5f;
  venusData.orbitSpeed = 35.0f;
  venusData.selfRotationSpeed = 3.0f;
  venusData.orbitColor = glm::vec3(0.9f, 0.8f, 0.5f);
  celestialBodies_.push_back(venusData);

  // --- ZIEMIA ---
  auto earthNode = std::make_shared<GraphNode>(earthModel_.get());
  earthNode->getTransform().setScale(glm::vec3(0.13f));
  rootNode_->addChild(earthNode);

  CelestialBody earthData;
  earthData.node = earthNode;
  earthData.orbitRadius = 6.0f;
  earthData.orbitSpeed = 25.0f;
  earthData.selfRotationSpeed = 30.0f;
  earthData.orbitColor = glm::vec3(0.0f, 0.0f, 1.0f);
  celestialBodies_.push_back(earthData);

  // --- KSIĘŻYC (Ziemi) ---
  auto moonNode = std::make_shared<GraphNode>(moonModel_.get());
  moonNode->getTransform().setScale(glm::vec3(0.25f));
  earthNode->addChild(moonNode);

  CelestialBody moonData;
  moonData.node = moonNode;
  moonData.orbitRadius = 1.8f;
  moonData.orbitSpeed = 80.0f;
  moonData.selfRotationSpeed = 0.0f;
  moonData.orbitColor = glm::vec3(0.5f, 0.5f, 0.5f);
  celestialBodies_.push_back(moonData);

  // --- MARS ---
  auto marsNode = std::make_shared<GraphNode>(marsModel_.get());
  marsNode->getTransform().setScale(glm::vec3(0.1f));
  rootNode_->addChild(marsNode);

  CelestialBody marsData;
  marsData.node = marsNode;
  marsData.orbitRadius = 8.0f;
  marsData.orbitSpeed = 20.0f;
  marsData.selfRotationSpeed = 25.0f;
  marsData.orbitColor = glm::vec3(1.0f, 0.2f, 0.0f);
  celestialBodies_.push_back(marsData);

  // --- JOWISZ ---
  auto jupiterNode = std::make_shared<GraphNode>(jupiterModel_.get());
  jupiterNode->getTransform().setScale(glm::vec3(0.6f));
  rootNode_->addChild(jupiterNode);

  CelestialBody jupiterData;
  jupiterData.node = jupiterNode;
  jupiterData.orbitRadius = 13.0f;
  jupiterData.orbitSpeed = 12.0f;
  jupiterData.selfRotationSpeed = 50.0f;
  jupiterData.orbitColor = glm::vec3(0.8f, 0.5f, 0.3f);
  celestialBodies_.push_back(jupiterData);

  // Jupiter: 1st moon
  auto jupiterMoonOneNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonOneNode->getTransform().setScale(glm::vec3(0.12f));
  jupiterNode->addChild(jupiterMoonOneNode);
  CelestialBody jupiterMoonOneData;
  jupiterMoonOneData.node = jupiterMoonOneNode;
  jupiterMoonOneData.orbitRadius = 1.8f;
  jupiterMoonOneData.orbitSpeed = 80.0f;
  jupiterMoonOneData.selfRotationSpeed = 0.0f;
  jupiterMoonOneData.orbitColor = glm::vec3(0.9f, 0.8f, 0.0f);
  celestialBodies_.push_back(jupiterMoonOneData);

  // Jupiter: 2nd moon
  auto jupiterMoonTwoNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonTwoNode->getTransform().setScale(glm::vec3(0.1f));
  jupiterNode->addChild(jupiterMoonTwoNode);
  CelestialBody jupiterMoonTwoData;
  jupiterMoonTwoData.node = jupiterMoonTwoNode;
  jupiterMoonTwoData.orbitRadius = 2.8f;
  jupiterMoonTwoData.orbitSpeed = 50.0f;
  jupiterMoonTwoData.selfRotationSpeed = 0.0f;
  jupiterMoonTwoData.orbitColor = glm::vec3(0.8f, 0.8f, 1.0f);
  celestialBodies_.push_back(jupiterMoonTwoData);

  // Jupiter: 3th moon
  auto jupiterMoonThreeNode =
      std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonThreeNode->getTransform().setScale(glm::vec3(0.15f));
  jupiterNode->addChild(jupiterMoonThreeNode);

  CelestialBody jupiterMoonThreeData;
  jupiterMoonThreeData.node = jupiterMoonThreeNode;
  jupiterMoonThreeData.orbitRadius = 4.5f;
  jupiterMoonThreeData.orbitSpeed = 30.0f;
  jupiterMoonThreeData.selfRotationSpeed = 0.0f;
  jupiterMoonThreeData.orbitColor = glm::vec3(0.7f, 0.7f, 0.7f);
  celestialBodies_.push_back(jupiterMoonThreeData);

  // Jupiter: 4th moon
  auto jupiterMoonFourNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonFourNode->getTransform().setScale(glm::vec3(0.13f));
  jupiterNode->addChild(jupiterMoonFourNode);

  CelestialBody jupiterMoonFourData;
  jupiterMoonFourData.node = jupiterMoonFourNode;
  jupiterMoonFourData.orbitRadius = 6.0f;
  jupiterMoonFourData.orbitSpeed = 20.0f;
  jupiterMoonFourData.selfRotationSpeed = 0.0f;
  jupiterMoonFourData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(jupiterMoonFourData);

  // --- SATURN ---
  auto saturnNode = std::make_shared<GraphNode>(saturnModel_.get());
  saturnNode->getTransform().setScale(glm::vec3(0.5f));
  rootNode_->addChild(saturnNode);

  CelestialBody saturnData;
  saturnData.node = saturnNode;
  saturnData.orbitRadius = 17.0f;
  saturnData.orbitSpeed = 9.0f;
  saturnData.selfRotationSpeed = 45.0f;
  saturnData.orbitColor = glm::vec3(0.9f, 0.8f, 0.2f);
  celestialBodies_.push_back(saturnData);

  // Saturn : first moon
  auto saturnMoonOneNode = std::make_shared<GraphNode>(saturnMoonOne_.get());
  saturnMoonOneNode->getTransform().setScale(glm::vec3(0.13f));
  saturnNode->addChild(saturnMoonOneNode);

  CelestialBody saturnMoonOneData;
  saturnMoonOneData.node = saturnMoonOneNode;
  saturnMoonOneData.orbitRadius = 3.0f;
  saturnMoonOneData.orbitSpeed = 20.0f;
  saturnMoonOneData.selfRotationSpeed = 0.0f;
  saturnMoonOneData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(saturnMoonOneData);

  // Saturn : second moon
  auto saturnMoonTwoNode = std::make_shared<GraphNode>(saturnMoonOne_.get());
  saturnMoonTwoNode->getTransform().setScale(glm::vec3(0.13f));
  saturnNode->addChild(saturnMoonTwoNode);

  CelestialBody saturnMoonTwoData;
  saturnMoonTwoData.node = saturnMoonTwoNode;
  saturnMoonTwoData.orbitRadius = 6.0f;
  saturnMoonTwoData.orbitSpeed = 20.0f;
  saturnMoonTwoData.selfRotationSpeed = 0.0f;
  saturnMoonTwoData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(saturnMoonTwoData);

  // --- URAN ---
  auto uranNode = std::make_shared<GraphNode>(uranModel_.get());
  uranNode->getTransform().setScale(glm::vec3(0.3f));
  rootNode_->addChild(uranNode);

  CelestialBody uranData;
  uranData.node = uranNode;
  uranData.orbitRadius = 22.0f;
  uranData.orbitSpeed = 6.0f;
  uranData.selfRotationSpeed = 20.0f;
  uranData.orbitColor = glm::vec3(0.0f, 1.0f, 1.0f);
  celestialBodies_.push_back(uranData);

  // --- NEPTUN ---
  auto neptuneNode = std::make_shared<GraphNode>(neptuneModel_.get());
  neptuneNode->getTransform().setScale(glm::vec3(0.3f));
  rootNode_->addChild(neptuneNode);

  CelestialBody neptuneData;
  neptuneData.node = neptuneNode;
  neptuneData.orbitRadius = 26.0f;
  neptuneData.orbitSpeed = 4.0f;
  neptuneData.selfRotationSpeed = 22.0f;
  neptuneData.orbitColor = glm::vec3(0.0f, 0.0f, 0.8f);
  celestialBodies_.push_back(neptuneData);
}

void SolarSystem::drawOrbits() {
  if (!orbitModel_ || celestialBodies_.empty())
    return;

  glLineWidth(1.0f);
  shader_->use();
  shader_->setUniform("useTexture", false);

  for (const auto &body : celestialBodies_) {
    if (body.orbitRadius <= 0.0f)
      continue;

    glm::mat4 orbitModelMatrix = glm::mat4(1.0f);
    glm::vec3 centerPos = glm::vec3(0.0f);

    float visualRadius = body.orbitRadius;
    GraphNode *parentNode = body.node->getParent();

    if (parentNode && parentNode != rootNode_.get()) {
      centerPos = parentNode->getGlobalPosition();
      glm::vec3 parentScale = parentNode->getTransform().getScale();
      float parentUniformScale = parentScale.x;

      visualRadius *= parentUniformScale;
    }
    orbitModelMatrix = glm::translate(orbitModelMatrix, centerPos);
    orbitModelMatrix = glm::scale(orbitModelMatrix, glm::vec3(visualRadius));

    shader_->setUniform("model", orbitModelMatrix);
    shader_->setUniform("objectColor", body.orbitColor);

    orbitModel_->draw(*shader_);
  }
}

Model *SolarSystem::generateSphereModel(GLuint sectorCount, GLuint stackCount) {
  const float radius = 1.0f;

  std::vector<Vertex> vertices;
  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / radius;
  float s, t;

  float sectorStep = 2 * M_PI / sectorCount;
  float stackStep = M_PI / stackCount;
  float sectorAngle, stackAngle;

  for (GLuint i = 0; i <= stackCount; ++i) {
    stackAngle = M_PI / 2 - i * stackStep;
    xy = radius * cosf(stackAngle);
    z = radius * sinf(stackAngle);

    for (GLuint j = 0; j <= sectorCount; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle);
      y = xy * sinf(sectorAngle);

      s = (float)j / sectorCount;
      t = (float)i / stackCount;

      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;

      Vertex newVertex;
      newVertex.position = glm::vec3(x, y, z);
      newVertex.normal = glm::vec3(nx, ny, nz);
      newVertex.texCords = glm::vec2(s, t);

      newVertex.tangent = glm::vec3(0.0f);

      vertices.push_back(newVertex);
    }
  }

  std::vector<unsigned int> indices;
  GLuint k1, k2;

  for (GLuint i = 0; i < stackCount; ++i) {
    k1 = i * (sectorCount + 1);
    k2 = (i + 1) * (sectorCount + 1);

    for (GLuint j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }

  std::vector<Texture> textures;
  Mesh mesh(vertices, indices, textures);

  Model *model = new Model();
  model->addMesh(mesh);
  return model;
}

Model *SolarSystem::generateOrbitModel(int segments) {
  std::vector<Vertex> orbitVertices;
  std::vector<unsigned int> indices;
  const float radius = 1.0f;
  const float step = 2 * M_PI / segments;

  // Create vertices for the orbit circle
  for (int i = 0; i <= segments; ++i) {
    float angle = i * step;
    float x = radius * cosf(angle);
    float z = radius * sinf(angle);

    Vertex v;
    v.position = glm::vec3(x, 0.0f, z);
    v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
    v.texCords = glm::vec2(0.0f);
    v.tangent = glm::vec3(0.0f);
    orbitVertices.push_back(v);
  }

  // Create line strip indices
  for (int i = 0; i < segments; ++i) {
    indices.push_back(i);
    indices.push_back(i + 1);
  }

  std::vector<Texture> textures;
  Mesh mesh(orbitVertices, indices, textures);
  mesh.setDrawMode(GL_LINES);

  Model *model = new Model();
  model->addMesh(mesh);
  return model;
}

void SolarSystem::regenerateGeneratedModels() {
  jupiterMoonOne_.reset();
  saturnMoonOne_.reset();

  GLuint tessellation = static_cast<GLuint>(currentTessellation_);

  jupiterMoonOne_ =
      std::unique_ptr<Model>(generateSphereModel(tessellation, tessellation));
  saturnMoonOne_ =
      std::unique_ptr<Model>(generateSphereModel(tessellation, tessellation));

  createScene();
}
