#include "housingEstate.h"
#include "GLFW/glfw3.h"
#include "core/mesh.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

HousingEstate::HousingEstate() {}
HousingEstate::~HousingEstate() {}

void HousingEstate::init_app() {
  std::string resourceDir = "src/apps/housing-estate/res/";
  shader_ = new Shader(resourceDir + "shaders/basic.vert",
                       resourceDir + "shaders/basic.frag");
  camera_ = new Camera(Camera::Perspective(glm::vec3(0.0f, 20.0f, 0.0f),
                                           glm::radians(45.0f),
                                           1920.0f / 1080.0f, 0.1f, 100.0f));
  createScene();

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
      firstMouse = true; // Zresetuj flagę, aby uniknąć skoku kamery
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
  float dt = deltaTime;
  for (auto &house : houses_) {
  }
  if (rootNode_) {
    rootNode_->updateTransform(glm::mat4(1.0f));
  }
}

void HousingEstate::render() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 view = camera_->getViewMatrix();
  glm::mat4 projection = camera_->getProjection();

  shader_->use();
  shader_->setUniform("view", view);
  shader_->setUniform("projection", projection);

  if (rootNode_) {
    rootNode_->draw(*shader_);
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void HousingEstate::render_gui() {
  ImGui::Begin("Controls");
  ImGui::End();
}

void HousingEstate::createScene() {
  rootNode_ = std::make_shared<GraphNode>();

  // PLANE BASE FOR HOUSES
  std::string textureDir = "src/apps/housing-estate/res/textures";
  std::string textureFile = "grass.jpg";
  Model *planeModel = generatePlaneModel(200.0f, textureDir, textureFile);
  auto planeNode = std::make_shared<GraphNode>(planeModel);
  rootNode_->addChild(planeNode);
  //
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
