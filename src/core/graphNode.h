#pragma once

#include "core/model.h"
#include "core/shader.h"
#include "core/transform.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include <vector>

class GraphNode {
public:
  GraphNode(Model *model = nullptr, GraphNode *parent = nullptr)
      : model_(model), parent_(parent) {}

  virtual void draw(Shader &shader);
  void addChild(std::shared_ptr<GraphNode> child);

  Transform &getTransform();
  void updateTransform(const glm::mat4 &parentGlobalTransform);

  GraphNode *getParent();
  std::vector<std::shared_ptr<GraphNode>> getChildren();
  Model *getModel();

  glm::mat4 getGlobalTransform();
  glm::vec3 getGlobalPosition() const;

  void setIsInstancedRendering(bool isInstanced);

private:
  GraphNode *parent_ = nullptr;
  std::vector<std::shared_ptr<GraphNode>> children_;
  Model *model_;

  Transform transform_;
  glm::mat4 globalTransform_ = glm::mat4(1.0f);

  bool isInstancedRender_ = false;
};
