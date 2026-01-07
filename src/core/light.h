#pragma once
#include "core/shader.h"
#include "core/transform.h"
#include "glm/ext/vector_float3.hpp"
#include <string>

enum class LightType { DIRECTIONAL = 0, POINT = 1, SPOT = 2 };

class Light {
public:
  Light(std::string name, LightType type);

  void sendToShader(Shader &shader, int index);

  const std::string &getName() const { return name_; }
  LightType getType() const { return type_; }
  bool isEnabled() const { return enabled_; }
  const glm::vec3 &getColor() const { return color_; }
  float getIntensity() const { return intensity_; }
  Transform &getTransform() { return transform_; }
  const Transform &getTransform() const { return transform_; }
  float getRange() const { return range_; }
  float getCutOff() const { return cutOff_; }
  float getOuterCutOff() const { return outerCutOff_; }

  void setEnabled(bool enabled) { enabled_ = enabled; }
  void setColor(const glm::vec3 &color) { color_ = color; }
  void setIntensity(float intensity) { intensity_ = intensity; }
  void setRange(float range) { range_ = range; }
  void setCutOff(float cutOff) { cutOff_ = cutOff; }
  void setOuterCutOff(float outerCutOff) { outerCutOff_ = outerCutOff; }

private:
  std::string name_;
  LightType type_;
  bool enabled_ = true;

  glm::vec3 color_ = glm::vec3(1.0f);
  float intensity_ = 1.0f;

  Transform transform_;

  // POINT AND SPOT
  float range_ = 50.0f;

  // SPOT
  float cutOff_ = 10.0f;
  float outerCutOff_ = 20.0f;
};
