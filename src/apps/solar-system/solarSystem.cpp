#include "solarSystem.h"
#include <iostream>

SolarSystem::SolarSystem() {}
SolarSystem::~SolarSystem() {}

void SolarSystem::init_app() {
  std::string resourceDir = "src/apps/solar-system/res/";
  shader_ = new Shader(resourceDir + "shaders/basic.vert",
                       resourceDir + "shaders/basic.frag");
  camera_ = new Camera(Camera::Perspective(glm::vec3(0.0f, 5.0f, 15.0f),
                                           glm::radians(45.0f),
                                           1920.0f / 1080.0f, 0.1f, 100.0f));

  loadedModel_ =
      std::make_unique<Model>(resourceDir + "models/earth/scene.gltf");

  // loadedModel_ = std::unique_ptr<Model>(generateSphereModel(32, 32));
  orbitModel_ = std::unique_ptr<Model>(generateOrbitModel(64));
  // generatedSunModel_ = std::unique_ptr<Model>(generateSphereModel(32, 32));
  // generatedMoonModel_ = std::unique_ptr<Model>(generateSphereModel(32, 32));

  createScene();
  glEnable(GL_DEPTH_TEST);
}

void SolarSystem::input() {
  GLFWwindow *glfwWin = window->getWindow();
  if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement ::FORWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement ::BACKWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement ::RIGHT, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
    camera_->processKeyboard(CameraMovement ::LEFT, deltaTime);
}

void SolarSystem::update() {
  float dt = deltaTime * globalSpeedMultiplier_;

  for (auto &body : celestialBodies_) {
    body.currentOrbitAngle += body.orbitSpeed * dt;
    body.currentSelfAngle += body.selfRotationSpeed * dt;
    if (body.orbitRadius > 0.0f) {
      float x = glm::cos(body.currentOrbitAngle) * body.orbitRadius;
      float z = glm::sin(body.currentOrbitAngle) * body.orbitRadius;
      body.node->getTransform().setPosition(glm::vec3(x, 0.0f, z));
      // std::cout << "New position: (" << x << ", 0.0, " << z << ")" <<
      // std::endl;
    }
    body.node->getTransform().setRotation(
        glm::vec3(0.0f, body.currentSelfAngle, 0.0f));
  }
  if (rootNode_) {
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

  if (rootNode_) {
    rootNode_->draw(*shader_);
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  drawOrbits();
}

void SolarSystem::render_gui() {
  ImGui::Begin("Controls");
  ImGui::End();
}

void SolarSystem::createScene() {
  celestialBodies_.clear();

  rootNode_ = std::make_shared<GraphNode>(nullptr);

  // auto sunNode = std::make_shared<GraphNode>(generatedSunModel_.get());
  // sunNode->getTransform().setScale(glm::vec3(3.0f));
  // rootNode_->addChild(sunNode);
  //
  // celestialBodies_.push_back({sunNode, 0.0f, 0.0f, 5.0f});

  auto earthNode = std::make_shared<GraphNode>(loadedModel_.get());
  earthNode->getTransform().setScale(glm::vec3(0.5f));
  rootNode_->addChild(earthNode);

  CelestialBody earthData;
  earthData.node = earthNode;
  earthData.orbitRadius = 20.0f;
  earthData.orbitSpeed = 10.0f;
  earthData.selfRotationSpeed = 30.0f;
  earthData.orbitColor = glm::vec3(0.0f, 0.0f, 1.0f);
  celestialBodies_.push_back(earthData);

  // auto moonNode = std::make_shared<GraphNode>(generatedSunModel_.get());
  // moonNode->getTransform().setScale(glm::vec3(0.2f));
  // earthNode->addChild(moonNode);

  // CelestialBody moonData;
  // moonData.node = moonNode;
  // moonData.orbitRadius = 2.0f;
  // moonData.orbitSpeed = 50.0f;
  // moonData.selfRotationSpeed = 0.0f;
  // celestialBodies_.push_back(moonData);
}

void SolarSystem::drawOrbits() {
  if (!orbitModel_ || celestialBodies_.empty())
    return;

  glLineWidth(1.0f);
  shader_->use();

  for (const auto &body : celestialBodies_) {
    if (body.orbitRadius <= 0.0f)
      continue;

    glm::mat4 orbitModelMatrix = glm::mat4(1.0f);

    glm::vec3 centerPos = glm::vec3(0.0f);

    orbitModelMatrix = glm::translate(orbitModelMatrix, centerPos);
    orbitModelMatrix =
        glm::scale(orbitModelMatrix, glm::vec3(body.orbitRadius));

    shader_->setUniform("model", orbitModelMatrix);
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
  std::vector<unsigned int> indices; // This was empty before!
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
