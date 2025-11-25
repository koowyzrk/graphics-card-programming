#include "graphNode.h"

GraphNode::GraphNode() : transform_(1.0f) {}

GraphNode::GraphNode(const glm::mat4 &transform) : transform_(transform) {}

void GraphNode::addMesh(const Mesh &mesh) { meshes_.push_back(mesh); }

void GraphNode::addChild(const GraphNode &child) { children_.push_back(child); }

const glm::mat4 &GraphNode::getTransform() const { return transform_; }

void GraphNode::setTransform(const glm::mat4 &transform) {
  transform_ = transform;
}

const std::vector<Mesh> &GraphNode::getMeshes() const { return meshes_; }

const std::vector<GraphNode> &GraphNode::getChildren() const {
  return children_;
}
