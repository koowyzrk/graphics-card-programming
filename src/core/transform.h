#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
class Transform {
public:
  Transform()
      : position_(0.0f), rotation_(0.0f), scale_(1.0f), modelMatrix_(1.0f),
        isDirty_(true) {}

  void setPosition(const glm::vec3 &pos);
  void setRotation(const glm::vec3 &rot);
  void setScale(const glm::vec3 &scale);

  void translate(const glm::vec3 &delta);
  void rotate(const glm::vec3 &delta);

  const glm::vec3 &getPosition() const;
  const glm::vec3 &getRotation() const;
  const glm::vec3 &getScale() const;

  const glm::mat4 &getLocalMatrix();

private:
  glm::vec3 position_;
  glm::vec3 rotation_;
  glm::vec3 scale_;

  glm::mat4 modelMatrix_;
  bool isDirty_;
};
