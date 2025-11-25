#pragma once
#include "core/baseapp.h"
#include "core/graphNode.h"
#include "core/model.h"

class SolarSystem : public BaseApp {
public:
  SolarSystem();
  ~SolarSystem() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
  GraphNode root_;

  Model *loadedModel_;
  // Mesh sphereMesh_;

  float systemRotationAngle_ = 0.0f;
  float zoom_ = 50.0f;

  float planetAngles_[8];
  float moonAngles_[10];

  bool wireframe_ = false;
  int meshDetail_ = 32;

  Mesh generateSphere(int detail);
  GraphNode createPlanet(float distance, float size, float revolutionSpeed,
                         float rotationSpeed, const glm::vec3 &color);
};
