#include "graphNode.h"

void GraphNode::draw(Shader &shader) {
  if (model_ && !isInstancedRender_) {
    shader.setUniform("model", globalTransform_);
    model_->draw(shader);
  }
  for (auto &child : children_) {
    child->draw(shader);
  }
}

void GraphNode::addChild(std::shared_ptr<GraphNode> child) {
  child->parent_ = this;
  children_.push_back(child);
}

Transform &GraphNode::getTransform() { return transform_; }
void GraphNode::updateTransform(const glm::mat4 &parentGlobalTransform) {
  const glm::mat4 &localMat = transform_.getLocalMatrix();
  globalTransform_ = parentGlobalTransform * localMat;
  for (auto &child : children_) {
    child->updateTransform(globalTransform_);
  }
}

GraphNode *GraphNode::getParent() { return parent_; }
std::vector<std::shared_ptr<GraphNode>> GraphNode::getChildren() {
  return children_;
}
Model *GraphNode::getModel() { return model_; }

glm::mat4 GraphNode::getGlobalTransform() { return globalTransform_; }
glm::vec3 GraphNode::getGlobalPosition() const {
  return glm::vec3(globalTransform_[3]);
}

void GraphNode::setIsInstancedRendering(bool isInstanced) {
  isInstancedRender_ = isInstanced;
}
