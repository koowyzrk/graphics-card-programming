#pragma once
#include "core/baseapp.h"
#include "core/camera.h"
#include "core/graphNode.h"
#include "glm/ext/vector_float3.hpp"

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

  std::unique_ptr<Model> loadedModel_;
  std::unique_ptr<Model> generatedSunModel_;
  std::unique_ptr<Model> generatedMoonModel_;
  std::unique_ptr<Model> orbitModel_;

  std::vector<CelestialBody> celestialBodies_;

  bool wireframeMode_ = false;
  float globalSpeedMultiplier_ = 1.0f;
  int sphereDetail_ = 32;

  void drawOrbits();
  void createScene();
  Model *generateSphereModel(GLuint sectorCount, GLuint stackCount);
  Model *generateOrbitModel(int segments);
};
