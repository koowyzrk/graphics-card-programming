#pragma once

#include "core/shader.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include <string>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCords;
  glm::vec3 tangent;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

class Mesh {
public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures);
  void draw(Shader &shader) const;
  void setDrawMode(GLenum drawMode);

private:
  unsigned int VAO, VBO, EBO;
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;
  GLenum drawMode_ = GL_TRIANGLES;

  void setupMesh();
};
