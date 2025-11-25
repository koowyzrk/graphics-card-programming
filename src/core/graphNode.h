#pragma once

#include "core/mesh.h"
#include "glm/ext/matrix_float4x4.hpp"
#include <vector>
class GraphNode {
public:
  GraphNode();
  GraphNode(const glm::mat4 &transform);

  void addMesh(const Mesh &mesh);
  void addChild(const GraphNode &child);

  const glm::mat4 &getTransform() const;
  void setTransform(const glm::mat4 &transform);

  const std::vector<Mesh> &getMeshes() const;
  const std::vector<GraphNode> &getChildren() const;

private:
  glm::mat4 transform_;
  std::vector<Mesh> meshes_;
  std::vector<GraphNode> children_;
};
