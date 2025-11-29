#include "transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

void Transform::setPosition(const glm::vec3 &pos) {
  position_ = pos;
  isDirty_ = true;
}
void Transform::setRotation(const glm::vec3 &rot) {
  rotation_ = rot;
  isDirty_ = true;
}
void Transform::setScale(const glm::vec3 &scale) {
  scale_ = scale;
  isDirty_ = true;
}

void Transform::translate(const glm::vec3 &delta) {
  position_ += delta;
  isDirty_ = true;
}

void Transform::rotate(const glm::vec3 &delta) {
  rotation_ += delta;
  isDirty_ = true;
}

const glm::vec3 &Transform::getPosition() const { return position_; }
const glm::vec3 &Transform::getRotation() const { return rotation_; }
const glm::vec3 &Transform::getScale() const { return scale_; }

const glm::mat4 &Transform::getLocalMatrix() {
  if (isDirty_) {
    modelMatrix_ = glm::mat4(1.0f);
    modelMatrix_ = glm::translate(modelMatrix_, position_);

    modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.x),
                               glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.y),
                               glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix_ = glm::rotate(modelMatrix_, glm::radians(rotation_.z),
                               glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix_ = glm::scale(modelMatrix_, scale_);

    isDirty_ = false;
  }
  return modelMatrix_;
}
