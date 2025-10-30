#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <cmath>

Camera Camera::Perspective(glm::vec3 position, float fov, float aspect,
                           float zNear, float zFar) {
  Camera camera(position, CameraProjection::Perspective);
  camera.fov_ = fov;
  camera.aspect_ = aspect;
  camera.zNear_ = zNear;
  camera.zFar_ = zFar;
  camera.projection_ = glm::perspective(fov, aspect, zNear, zFar);
  camera.updateCameraVectors();
  return camera;
}

Camera Camera::Orthographic(glm::vec3 position, float left, float right,
                            float bottom, float top, float zNear, float zFar) {
  Camera camera(position, CameraProjection::Orthographic);
  camera.orthoLeft_ = left;
  camera.orthoRight_ = right;
  camera.orthoBottom_ = bottom;
  camera.orthoTop_ = top;
  camera.zNear_ = zNear;
  camera.zFar_ = zFar;
  camera.projection_ = glm::ortho(left, right, bottom, top, zNear, zFar);
  camera.updateCameraVectors();
  return camera;
}

Camera::Camera(glm::vec3 position, CameraProjection type)
    : position_(position), projectionType_(type) {}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position_, position_ + direction_, up_);
}

void Camera::setProjection(float fov, float aspect, float zNear, float zFar) {
  projectionType_ = CameraProjection::Perspective;
  projection_ = glm::perspective(fov, aspect, zNear, zFar);
}

void Camera::setProjection(float left, float right, float bottom, float top,
                           float zNear, float zFar) {
  projectionType_ = CameraProjection::Orthographic;
  projection_ = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::processKeyboard(CameraMovement direction, float dt) {
  float cameraVelocity = cameraSpeed * dt;
  if (direction == FORWARD) {
    position_ += direction_ * cameraVelocity;
  } else if (direction == BACKWARD) {
    position_ -= direction_ * cameraVelocity;
  } else if (direction == RIGHT) {
    position_ += right_ * cameraVelocity;
  } else if (direction == LEFT) {
    position_ -= right_ * cameraVelocity;
  }
}
void Camera::processMouseMovement(float xOffset, float yOffset) {
  xOffset *= mouseSensitivity;
  yOffset *= mouseSensitivity;

  yaw_ += xOffset;
  pitch_ += yOffset;

  if (pitch_ > 89.0f)
    pitch_ = 89.0f;
  if (pitch_ < -89.0f)
    pitch_ = -89.0f;

  updateCameraVectors();
}
void Camera::processMouseZoom(float yOffset) {
  fov_ -= yOffset;
  if (fov_ < 1.0f)
    fov_ = 1.0f;
  if (fov_ > 45.0f)
    fov_ = 45.0f;
}

void Camera::updateCameraVectors() {
  glm::vec3 direction =
      glm::vec3(cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
                sin(glm::radians(pitch_)),
                sin(glm::radians(yaw_)) * cos(glm::radians(pitch_)));
  direction_ = glm::normalize(direction);
  right_ = glm::normalize(glm::cross(direction_, worldUp_));
  up_ = glm::normalize(glm::cross(right_, direction_));
}
