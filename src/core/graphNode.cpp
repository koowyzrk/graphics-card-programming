#include "graphNode.h"

void GraphNode::addChild(std::shared_ptr<GraphNode> child) {
  child->parent_ = this;
  children_.push_back(child);
}

std::vector<std::shared_ptr<GraphNode>> GraphNode::getChildren() {
  return children_;
}

Transform &GraphNode::getTransform() { return transform_; }

void GraphNode::updateTransform(const glm::mat4 &parentGlobalTransform) {
  const glm::mat4 &localMat = transform_.getLocalMatrix();
  globalTransform_ = parentGlobalTransform * localMat;
  // updating transforms in childs
  for (auto &child : children_) {
    child->updateTransform(globalTransform_);
  }
}

void GraphNode::draw(Shader &shader) {
  if (model_) {
    shader.setUniform("model", globalTransform_);
    model_->draw(shader);
  }
  for (auto &child : children_) {
    child->draw(shader);
  }
}

Model *GraphNode::getModel() { return model_; }
GraphNode *GraphNode::getParent() { return parent_; }
glm::mat4 GraphNode::getGlobalTransform() { return globalTransform_; }

glm::vec3 GraphNode::getGlobalPosition() const {
  return glm::vec3(globalTransform_[3]);
}
