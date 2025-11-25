#include "solarSystem.h"

SolarSystem::SolarSystem() {}
SolarSystem::~SolarSystem() {}

void SolarSystem::init_app() {
  loadedModel_ = new Model(
      std::string("src/apps/solar-system/res/models/earth/scene.gltf"));

  root_ = GraphNode(glm::mat4(1.0f));

  GraphNode earthNode(glm::mat4(1.0f));
  earthNode.addModel(*loadedModel_);

  root_.addChild(earthNode);
}

void SolarSystem::input() {}

void SolarSystem::update() {}

void SolarSystem::render() {}
