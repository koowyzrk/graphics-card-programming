#include "solarSystem.h"
#include "GLFW/glfw3.h"
#include "core/graphNode.h"
#include <memory>

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
  bool spaceCurrentlyPressed =
      (glfwGetKey(glfwWin, GLFW_KEY_SPACE) == GLFW_PRESS);
  if (spaceCurrentlyPressed && !tabPressedLastFrame_) {
    mouseControlEnabled_ = !mouseControlEnabled_;
    if (mouseControlEnabled_) {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      firstMouse = true; // Zresetuj flagę, aby uniknąć skoku kamery
    } else {
      glfwSetInputMode(glfwWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
  tabPressedLastFrame_ = spaceCurrentlyPressed;

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
    // ustawienie planet w scenie względem układu
    if (body.orbitRadius > 0.0f) {
      float angleRad = glm::radians(body.currentOrbitAngle);
      float x = glm::cos(angleRad) * body.orbitRadius;
      float z = glm::sin(angleRad) * body.orbitRadius;
      body.node->getTransform().setPosition(glm::vec3(x, 0.0f, z));
    }
    // ustawienie pochylenia i rotacji
    if (body.tiltNode) {
      glm::vec3 tiltRotation = glm::vec3(body.axialTilt, 0.0f, 0.0f);
      body.tiltNode->getTransform().setRotation(tiltRotation);
      if (body.rotationNode) {
        body.rotationNode->getTransform().setRotation(
            glm::vec3(0.0f, body.currentSelfAngle, 0.0f));
      }
    } else {
      body.node->getTransform().setRotation(
          glm::vec3(0.0f, body.currentSelfAngle, 0.0f));
    }
  }
  // ogolny obrot dla calego ukladu
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
    std::shared_ptr<GraphNode> drawNode;
    if (body.rotationNode) {
      drawNode = body.rotationNode;
    } else if (body.tiltNode) {
      drawNode = body.tiltNode;
    } else {
      drawNode = body.node;
    }
    if (!drawNode || !drawNode->getModel())
      continue;

    shader_->setUniform("model", drawNode->getGlobalTransform());
    shader_->setUniform("objectColor", body.orbitColor);

    bool isGeneratedMoon = (drawNode->getModel() == jupiterMoonOne_.get() ||
                            drawNode->getModel() == saturnMoonOne_.get());

    if (isGeneratedMoon) {
      shader_->setUniform("useTexture", false);
    } else {
      shader_->setUniform("useTexture", true);
    }
    drawNode->getModel()->draw(*shader_);
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

  // --- SUN ---
  auto sunOrbitalNode = std::make_shared<GraphNode>(nullptr);
  sunOrbitalNode->getTransform().setScale(glm::vec3(0.2f));
  rootNode_->addChild(sunOrbitalNode);

  auto sunTiltNode = std::make_shared<GraphNode>(nullptr);
  sunOrbitalNode->addChild(sunTiltNode);

  auto sunRotationNode = std::make_shared<GraphNode>(sunModel_.get());
  sunTiltNode->addChild(sunRotationNode);

  CelestialBody sunData;
  sunData.node = sunOrbitalNode;
  sunData.tiltNode = sunTiltNode;
  sunData.rotationNode = sunRotationNode;

  sunData.orbitRadius = 0.0f;
  sunData.orbitSpeed = 0.0f;
  sunData.selfRotationSpeed = 2.0f;
  sunData.axialTilt = 7.0f;
  sunData.orbitColor = glm::vec3(1.0f, 1.0f, 0.0f);
  celestialBodies_.push_back(sunData);

  // --- MERCURY ---
  auto mercuryOrbitalNode = std::make_shared<GraphNode>(nullptr);
  mercuryOrbitalNode->getTransform().setScale(glm::vec3(0.08f));
  rootNode_->addChild(mercuryOrbitalNode);

  auto mercuryTiltNode = std::make_shared<GraphNode>(nullptr);
  mercuryOrbitalNode->addChild(mercuryTiltNode);

  auto mercuryRotationNode = std::make_shared<GraphNode>(mercuryModel_.get());
  mercuryTiltNode->addChild(mercuryRotationNode);

  CelestialBody mercuryData;
  mercuryData.node = mercuryOrbitalNode;
  mercuryData.tiltNode = mercuryTiltNode;
  mercuryData.rotationNode = mercuryRotationNode;

  mercuryData.orbitRadius = 3.0f;
  mercuryData.orbitSpeed = 45.0f;
  mercuryData.selfRotationSpeed = 10.0f;
  mercuryData.axialTilt = 2.0f;
  mercuryData.orbitColor = glm::vec3(0.6f, 0.6f, 0.6f);
  celestialBodies_.push_back(mercuryData);

  // --- VENUS ---
  auto venusOrbitalNode = std::make_shared<GraphNode>(nullptr);
  venusOrbitalNode->getTransform().setScale(glm::vec3(0.001f));
  rootNode_->addChild(venusOrbitalNode);

  auto venusTiltNode = std::make_shared<GraphNode>(nullptr);
  venusOrbitalNode->addChild(venusTiltNode);

  auto venusRotationNode = std::make_shared<GraphNode>(venusModel_.get());
  venusTiltNode->addChild(venusRotationNode);

  CelestialBody venusData;
  venusData.node = venusOrbitalNode;
  venusData.tiltNode = venusTiltNode;
  venusData.rotationNode = venusRotationNode;

  venusData.orbitRadius = 4.5f;
  venusData.orbitSpeed = 35.0f;
  venusData.selfRotationSpeed = 13.0f;
  venusData.axialTilt = 170.0f;
  venusData.orbitColor = glm::vec3(0.9f, 0.8f, 0.5f);
  celestialBodies_.push_back(venusData);

  // --- EARTH ---
  auto earthOrbitalNode = std::make_shared<GraphNode>(nullptr);
  earthOrbitalNode->getTransform().setScale(glm::vec3(0.13f));
  rootNode_->addChild(earthOrbitalNode);

  auto earthTiltNode = std::make_shared<GraphNode>(nullptr);
  earthOrbitalNode->addChild(earthTiltNode);

  auto earthRotationNode = std::make_shared<GraphNode>(earthModel_.get());
  earthTiltNode->addChild(earthRotationNode);

  CelestialBody earthData;
  earthData.node = earthOrbitalNode;
  earthData.tiltNode = earthTiltNode;
  earthData.rotationNode = earthRotationNode;

  earthData.orbitRadius = 6.0f;
  earthData.orbitSpeed = 25.0f;
  earthData.selfRotationSpeed = 30.0f;
  earthData.axialTilt = 23.0f;
  earthData.orbitColor = glm::vec3(0.0f, 0.0f, 1.0f);
  celestialBodies_.push_back(earthData);

  // --- EARTH MOON ---
  auto moonNode = std::make_shared<GraphNode>(moonModel_.get());
  moonNode->getTransform().setScale(glm::vec3(0.001f));
  earthOrbitalNode->addChild(moonNode);

  CelestialBody moonData;
  moonData.node = moonNode;
  moonData.orbitRadius = 1.8f;
  moonData.orbitSpeed = 80.0f;
  moonData.selfRotationSpeed = 0.0f;
  moonData.orbitColor = glm::vec3(0.5f, 0.5f, 0.5f);
  celestialBodies_.push_back(moonData);

  // --- MARS ---
  auto marsOrbitalNode = std::make_shared<GraphNode>(nullptr);
  marsOrbitalNode->getTransform().setScale(glm::vec3(0.1f));
  rootNode_->addChild(marsOrbitalNode);

  auto martTiltNode = std::make_shared<GraphNode>(nullptr);
  marsOrbitalNode->addChild(martTiltNode);

  auto marsRotationNode = std::make_shared<GraphNode>(marsModel_.get());
  martTiltNode->addChild(marsRotationNode);

  CelestialBody marsData;
  marsData.node = marsOrbitalNode;
  marsData.tiltNode = martTiltNode;
  marsData.rotationNode = marsRotationNode;

  marsData.orbitRadius = 8.0f;
  marsData.orbitSpeed = 20.0f;
  marsData.selfRotationSpeed = 25.0f;
  marsData.axialTilt = 25.0f;
  marsData.orbitColor = glm::vec3(1.0f, 0.2f, 0.0f);
  celestialBodies_.push_back(marsData);

  // --- JUPITER ---
  auto jupiterOrbitalNode = std::make_shared<GraphNode>(nullptr);
  jupiterOrbitalNode->getTransform().setScale(glm::vec3(0.005f));
  earthOrbitalNode->addChild(jupiterOrbitalNode);

  auto jupiterTiltNode = std::make_shared<GraphNode>(nullptr);
  jupiterOrbitalNode->addChild(jupiterTiltNode);

  auto jupiterRotationNode = std::make_shared<GraphNode>(jupiterModel_.get());
  jupiterTiltNode->addChild(jupiterRotationNode);

  CelestialBody jupiterData;
  jupiterData.node = jupiterOrbitalNode;
  jupiterData.tiltNode = jupiterTiltNode;
  jupiterData.rotationNode = jupiterRotationNode;

  jupiterData.orbitRadius = 13.0f;
  jupiterData.orbitSpeed = 12.0f;
  jupiterData.selfRotationSpeed = 50.0f;
  jupiterData.axialTilt = 3.0f;
  jupiterData.orbitColor = glm::vec3(0.8f, 0.5f, 0.3f);
  celestialBodies_.push_back(jupiterData);

  // Jupiter: 1st moon
  auto jupiterMoonOneNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonOneNode->getTransform().setScale(glm::vec3(2.0f));
  jupiterOrbitalNode->addChild(jupiterMoonOneNode);

  CelestialBody jupiterMoonOneData;
  jupiterMoonOneData.node = jupiterMoonOneNode;
  jupiterMoonOneData.orbitRadius = 120.0f;
  jupiterMoonOneData.orbitSpeed = 80.0f;
  jupiterMoonOneData.selfRotationSpeed = 0.0f;
  jupiterMoonOneData.orbitColor = glm::vec3(0.9f, 0.8f, 0.0f);
  celestialBodies_.push_back(jupiterMoonOneData);

  // Jupiter: 2nd moon
  auto jupiterMoonTwoNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonTwoNode->getTransform().setScale(glm::vec3(2.0f));
  jupiterOrbitalNode->addChild(jupiterMoonTwoNode);
  CelestialBody jupiterMoonTwoData;
  jupiterMoonTwoData.node = jupiterMoonTwoNode;
  jupiterMoonTwoData.orbitRadius = 140.0f;
  jupiterMoonTwoData.orbitSpeed = 50.0f;
  jupiterMoonTwoData.selfRotationSpeed = 0.0f;
  jupiterMoonTwoData.orbitColor = glm::vec3(0.8f, 0.8f, 1.0f);
  celestialBodies_.push_back(jupiterMoonTwoData);

  // Jupiter: 3th moon
  auto jupiterMoonThreeNode =
      std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonThreeNode->getTransform().setScale(glm::vec3(2.0f));
  jupiterOrbitalNode->addChild(jupiterMoonThreeNode);

  CelestialBody jupiterMoonThreeData;
  jupiterMoonThreeData.node = jupiterMoonThreeNode;
  jupiterMoonThreeData.orbitRadius = 160.0f;
  jupiterMoonThreeData.orbitSpeed = 30.0f;
  jupiterMoonThreeData.selfRotationSpeed = 0.0f;
  jupiterMoonThreeData.orbitColor = glm::vec3(0.7f, 0.7f, 0.7f);
  celestialBodies_.push_back(jupiterMoonThreeData);

  // Jupiter: 4th moon
  auto jupiterMoonFourNode = std::make_shared<GraphNode>(jupiterMoonOne_.get());
  jupiterMoonFourNode->getTransform().setScale(glm::vec3(2.0f));
  jupiterOrbitalNode->addChild(jupiterMoonFourNode);

  CelestialBody jupiterMoonFourData;
  jupiterMoonFourData.node = jupiterMoonFourNode;
  jupiterMoonFourData.orbitRadius = 180.0f;
  jupiterMoonFourData.orbitSpeed = 20.0f;
  jupiterMoonFourData.selfRotationSpeed = 0.0f;
  jupiterMoonFourData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(jupiterMoonFourData);

  // --- SATURN ---
  auto saturnOrbitalNode = std::make_shared<GraphNode>(nullptr);
  saturnOrbitalNode->getTransform().setScale(glm::vec3(0.004f));
  rootNode_->addChild(saturnOrbitalNode);

  auto saturnTiltNode = std::make_shared<GraphNode>(nullptr);
  saturnOrbitalNode->addChild(saturnTiltNode);

  auto saturnRotationNode = std::make_shared<GraphNode>(saturnModel_.get());
  saturnTiltNode->addChild(saturnRotationNode);

  CelestialBody saturnData;
  saturnData.node = saturnOrbitalNode;
  saturnData.tiltNode = saturnTiltNode;
  saturnData.rotationNode = saturnRotationNode;

  saturnData.orbitRadius = 17.0f;
  saturnData.orbitSpeed = 9.0f;
  saturnData.selfRotationSpeed = 45.0f;
  saturnData.axialTilt = 27.0f;
  saturnData.orbitColor = glm::vec3(0.9f, 0.8f, 0.2f);
  celestialBodies_.push_back(saturnData);

  // Saturn : first moon
  auto saturnMoonOneNode = std::make_shared<GraphNode>(saturnMoonOne_.get());
  saturnMoonOneNode->getTransform().setScale(glm::vec3(2.0f));
  saturnOrbitalNode->addChild(saturnMoonOneNode);

  CelestialBody saturnMoonOneData;
  saturnMoonOneData.node = saturnMoonOneNode;
  saturnMoonOneData.orbitRadius = 130.0f;
  saturnMoonOneData.orbitSpeed = 20.0f;
  saturnMoonOneData.selfRotationSpeed = 0.0f;
  saturnMoonOneData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(saturnMoonOneData);

  // Saturn : second moon
  auto saturnMoonTwoNode = std::make_shared<GraphNode>(saturnMoonOne_.get());
  saturnMoonTwoNode->getTransform().setScale(glm::vec3(2.0f));
  saturnOrbitalNode->addChild(saturnMoonTwoNode);

  CelestialBody saturnMoonTwoData;
  saturnMoonTwoData.node = saturnMoonTwoNode;
  saturnMoonTwoData.orbitRadius = 160.0f;
  saturnMoonTwoData.orbitSpeed = 20.0f;
  saturnMoonTwoData.selfRotationSpeed = 0.0f;
  saturnMoonTwoData.orbitColor = glm::vec3(0.5f, 0.4f, 0.3f);
  celestialBodies_.push_back(saturnMoonTwoData);

  // --- URAN ---
  auto uranOrbitalNode = std::make_shared<GraphNode>(nullptr);
  uranOrbitalNode->getTransform().setScale(glm::vec3(0.003f));
  rootNode_->addChild(uranOrbitalNode);

  auto uranTiltNode = std::make_shared<GraphNode>(nullptr);
  uranOrbitalNode->addChild(uranTiltNode);

  auto uranRotationNode = std::make_shared<GraphNode>(uranModel_.get());
  uranTiltNode->addChild(uranRotationNode);

  CelestialBody uranData;
  uranData.node = uranOrbitalNode;
  uranData.tiltNode = uranTiltNode;
  uranData.rotationNode = uranRotationNode;

  uranData.orbitRadius = 22.0f;
  uranData.orbitSpeed = 6.0f;
  uranData.selfRotationSpeed = 20.0f;
  uranData.axialTilt = 40.0f;
  uranData.orbitColor = glm::vec3(0.0f, 1.0f, 1.0f);
  celestialBodies_.push_back(uranData);

  // --- NEPTUN ---
  auto neptuneOrbitalNode = std::make_shared<GraphNode>(nullptr);
  neptuneOrbitalNode->getTransform().setScale(glm::vec3(0.003f));
  rootNode_->addChild(neptuneOrbitalNode);

  auto neptuneTiltNode = std::make_shared<GraphNode>(nullptr);
  neptuneOrbitalNode->addChild(neptuneTiltNode);

  auto neptuneRotationNode = std::make_shared<GraphNode>(neptuneModel_.get());
  neptuneTiltNode->addChild(neptuneRotationNode);

  CelestialBody neptuneData;
  neptuneData.node = neptuneOrbitalNode;
  neptuneData.tiltNode = neptuneTiltNode;
  neptuneData.rotationNode = neptuneRotationNode;

  neptuneData.orbitRadius = 26.0f;
  neptuneData.orbitSpeed = 4.0f;
  neptuneData.selfRotationSpeed = 22.0f;
  neptuneData.axialTilt = 28.0f;
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
  // ustalenie promienia sfery
  const float radius = 1.0f;

  glm::vec4 baseColor(1.0f);
  std::vector<Vertex> vertices;
  float x, y, z, xy;
  // wektor normalny, odwrotnosc promienia
  float nx, ny, nz, lengthInv = 1.0f / radius;
  // polozenie tekstury
  float s, t;

  // przyrost kąta wzdłuż równika (poziom)
  float sectorStep = 2 * M_PI / sectorCount;
  // przyrost kąta wzdłuż południka (gora doł)
  float stackStep = M_PI / stackCount;
  float sectorAngle, stackAngle;

  // iterowanie przez segmenty w pionie od bieguna polnocnego do poludniowego
  for (GLuint i = 0; i <= stackCount; ++i) {
    // kąt biegunowy od 90 do -90
    stackAngle = M_PI / 2 - i * stackStep;
    // promień okręgu cos(0) ==> 1
    // najwiekszy promien na rowniku
    xy = radius * cosf(stackAngle);
    // wspolrzedna pionowa
    z = radius * sinf(stackAngle);

    // iterowanie przez segmenty poziome tworzac okrąg dla stacka
    for (GLuint j = 0; j <= sectorCount; ++j) {
      // dlugosc geograficzna (obrecz) => 360 stopni
      sectorAngle = j * sectorStep;

      // wspolrzedne obliczone w oparciu o promien na wysokosci xy
      x = xy * cosf(sectorAngle);
      y = xy * sinf(sectorAngle);

      // wspolrzedne tekstury
      // pion
      s = (float)j / sectorCount;
      // poziom
      t = (float)i / stackCount;

      // wektor normalny wierzcholka
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

  // iterowanie przez segmenty w pionie
  for (GLuint i = 0; i < stackCount; ++i) {
    // lewy gorny wierzcholek
    k1 = i * (sectorCount + 1);
    // lewy dolny wierzcholek nastepnego stacka
    k2 = (i + 1) * (sectorCount + 1);

    // iterowanie przez segmenty w poziomie
    for (GLuint j = 0; j < sectorCount; ++j, ++k1, ++k2) {
      // warunek na polnocny biegun
      // gorny trojkat
      if (i != 0) {
        indices.push_back(k1);     // gorny lewy
        indices.push_back(k2);     // dolny
        indices.push_back(k1 + 1); // gorny prawy
      }
      // warunek na poludniowy biegun
      // dolny trojkat
      if (i != (stackCount - 1)) {
        indices.push_back(k1 + 1); // gorny
        indices.push_back(k2);     // dolny lewy
        indices.push_back(k2 + 1); // dolny prawy
      }
    }
  }

  std::vector<Texture> textures;
  Mesh mesh(vertices, indices, textures, baseColor);

  Model *model = new Model();
  model->addMesh(mesh);
  return model;
}

Model *SolarSystem::generateOrbitModel(int segments) {
  std::vector<Vertex> orbitVertices;
  std::vector<unsigned int> indices;
  glm::vec4 baseColor(1.0f);
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
  Mesh mesh(orbitVertices, indices, textures, baseColor);
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
