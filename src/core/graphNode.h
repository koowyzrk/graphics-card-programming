#pragma once

#include "core/model.h"
#include "core/shader.h"
#include "core/transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include <vector>

class GraphNode {
public:
  GraphNode(Model *model = nullptr, GraphNode *parent = nullptr)
      : model_(model), parent_(parent) {}
  void addChild(std::shared_ptr<GraphNode> child);

  void draw(Shader &shader);

  // transform section
  Transform &getTransform();
  void updateTransform(const glm::mat4 &parentGlobalTransform);

  std::vector<std::shared_ptr<GraphNode>> getChildren();

  Model *getModel();
  GraphNode *getParent();
  glm::mat4 getGlobalTransform();
  glm::vec3 getGlobalPosition() const;

private:
  Model *model_;
  std::vector<std::shared_ptr<GraphNode>> children_;

  GraphNode *parent_ = nullptr;

  Transform transform_;
  glm::mat4 globalTransform_ = glm::mat4(1.0f);
};
