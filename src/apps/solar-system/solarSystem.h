#pragma once
#include "core/baseapp.h"
#include "core/camera.h"
#include "core/graphNode.h"
#include "glm/ext/vector_float3.hpp"
#include <memory>

struct CelestialBody {
  std::shared_ptr<GraphNode> node;

  float orbitRadius = 0.0f;
  float orbitSpeed = 0.0f;
  float selfRotationSpeed = 0.0f;

  float currentOrbitAngle = 0.0f;
  float currentSelfAngle = 0.0f;

  glm::vec3 orbitColor = glm::vec3(1.0f);
};

class SolarSystem : public BaseApp {
public:
  SolarSystem();
  ~SolarSystem() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;
  void render_gui() override;

private:
  std::shared_ptr<GraphNode> rootNode_;
  Shader *shader_ = nullptr;
  Camera *camera_ = nullptr;

  std::unique_ptr<Model> sunModel_;
  std::unique_ptr<Model> sunGenModel_;

  std::unique_ptr<Model> mercuryModel_;
  std::unique_ptr<Model> venusModel_;

  std::unique_ptr<Model> earthModel_;
  std::unique_ptr<Model> moonModel_;

  std::unique_ptr<Model> marsModel_;

  std::unique_ptr<Model> jupiterModel_;
  std::unique_ptr<Model> jupiterMoonOne_;
  std::unique_ptr<Model> jupiterMoonTwo_;
  std::unique_ptr<Model> jupiterMoonThree_;
  std::unique_ptr<Model> jupiterMoonFour_;

  std::unique_ptr<Model> saturnModel_;
  std::unique_ptr<Model> saturnMoonOne_;
  std::unique_ptr<Model> saturnMoonTwo_;

  std::unique_ptr<Model> uranModel_;
  std::unique_ptr<Model> neptuneModel_;

  std::unique_ptr<Model> orbitModel_;

  std::vector<CelestialBody> celestialBodies_;

  bool wireframeMode_ = false;
  float globalSpeedMultiplier_ = 1.0f;
  float sceneRotationAngle_ = 0.0f;
  int currentTessellation_ = 32;

  void drawOrbits();
  void createScene();
  Model *generateSphereModel(GLuint sectorCount, GLuint stackCount);
  Model *generateOrbitModel(int segments);
  void regenerateGeneratedModels();

  // mouse control
  bool mouseControlEnabled_ = true;
  bool tabPressedLastFrame_ = false;
  double lastX = 0.0;
  double lastY = 0.0;
  bool firstMouse = true;
};
