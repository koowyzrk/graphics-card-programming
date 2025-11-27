#pragma once

#include "core/model.h"
#include "core/shader.h"
#include "core/transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>
class GraphNode {
public:
  GraphNode(Model *model = nullptr) : model_(model) {}
  void addChild(std::shared_ptr<GraphNode> child);
  std::vector<std::shared_ptr<GraphNode>> getChildren();

  // transform section
  Transform &getTransform();
  void updateTransform(const glm::mat4 &parentGlobalTransform);
  //

  void draw(Shader &shader);

  glm::vec3 getGlobalPosition() const;

private:
  Model *model_;
  std::vector<std::shared_ptr<GraphNode>> children_;

  Transform transform_;
  glm::mat4 globalTransform_ = glm::mat4(1.0f);
};
