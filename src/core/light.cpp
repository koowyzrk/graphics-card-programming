#include "light.h"

Light::Light(std::string name, LightType type) : name_(name), type_(type) {
  // if (type == LightType::DIRECTIONAL) {
  //   transform_.setRotation(glm::radians(glm::vec3(-45.0f, -45.0f, 0.0f)));
  // }
}

void Light::sendToShader(Shader &shader, int index, Transform &worldTransform) {
  std::string base = "lights[" + std::to_string(index) + "].";
  shader.setUniform(base + "enabled", enabled_);
  shader.setUniform(base + "type", (int)type_);
  shader.setUniform(base + "color", color_ * intensity_);

  shader.setUniform(base + "position", worldTransform.getPosition());
  shader.setUniform(base + "direction", worldTransform.getForward());
  // shader.setUniform(base + "position", transform_.getPosition());
  // shader.setUniform(base + "direction", transform_.getForward());

  shader.setUniform(base + "range", range_);
  shader.setUniform(base + "cutOff", glm::cos(glm::radians(cutOff_)));
  shader.setUniform(base + "outerCutOff", glm::cos(glm::radians(outerCutOff_)));
}
