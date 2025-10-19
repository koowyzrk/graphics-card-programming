#pragma once
#include "core/baseapp.h"
#include "core/utils.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>

class Sierpinski : public BaseApp {
public:
  Sierpinski();
  ~Sierpinski() override;

  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
  void generatePyramid(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
                       int depth);
  void addTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

  GLuint VAO = 0;
  GLuint VBO = 0;
  Shader *shader = nullptr;
  GLuint texture = 0;

  std::vector<float> vertices;
  std::vector<unsigned int> indicies;

  int recursionLevel = 0;
  int lastRecursionLevel = -1;
  float rotationX = 0.0f;
  float rotationY = 0.0f;
  glm::vec3 fracColor = {1.0f, 1.0f, 1.0f};

  float s = 1.0f;
  glm::vec3 A = glm::vec3(s / 2.0f, 0.0f, -s / (2.0f * glm::sqrt(3.0f)));
  glm::vec3 B = glm::vec3(-s / 2.0f, 0.0f, -s / (2.0f * glm::sqrt(3.0f)));
  glm::vec3 C = glm::vec3(0.0f, 0.0f, s / (glm::sqrt(3.0f)));
  glm::vec3 D = glm::vec3(0.0f, s *glm::sqrt(6.0f) / 3.0f, 0.0f);
};
