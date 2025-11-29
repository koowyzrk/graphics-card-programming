#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };
enum CameraProjection { Perspective, Orthographic };

class Camera {
public:
  // constructor for perspective projection
  static Camera Perspective(glm::vec3 position, float fov, float aspect,
                            float zNear = 0.1f, float zFar = 100.0f);
  // constructor for orthographic projection
  static Camera Orthographic(glm::vec3 position, float left, float right,
                             float bottom, float top, float zNear = 0.1f,
                             float zFar = 100.0f);

  glm::vec3 getPosition() const { return position_; }
  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjection() const { return projection_; }

  // perspective projection
  void setProjection(float fov, float aspect, float zNear, float zFar);
  // ortographic projection
  void setProjection(float left, float right, float bottom, float top,
                     float zNear, float zFar);

  float cameraSpeed = 2.5f;
  float mouseSensitivity = 0.1f;
  void processKeyboard(CameraMovement direction, float dt);
  void processMouseMovement(float xOffset, float yOffset);
  void processMouseZoom(float yOffset);

private:
  Camera(glm::vec3 position, CameraProjection type);

  glm::vec3 position_ = glm::vec3(0.0f);
  glm::vec3 direction_ = glm::vec3(0.0f);
  glm::vec3 right_ = glm::vec3(0.0f);
  glm::vec3 up_ = glm::vec3(0.0f);
  glm::vec3 worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f);

  CameraProjection projectionType_;
  glm::mat4 projection_ = glm::mat4(1.0f);

  float yaw_ = -90.0f;
  float pitch_ = 0.0f;

  float fov_, aspect_;
  float orthoLeft_, orthoRight_, orthoBottom_, orthoTop_;
  float zNear_, zFar_;

  void updateCameraVectors();
};
