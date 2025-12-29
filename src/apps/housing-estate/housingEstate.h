#pragma once
#include "core/baseapp.h"
#include "core/camera.h"
#include "core/graphNode.h"
#include <memory>

class HousingEstate : public BaseApp {
public:
  HousingEstate();
  ~HousingEstate() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;
  void render_gui() override;

private:
  std::shared_ptr<GraphNode> rootNode_;
  Shader *shader_ = nullptr;
  Camera *camera_ = nullptr;

  // HousingEstate
  std::vector<GraphNode> houses_; // for a moment
  void createScene();
  //

  // mouse control
  bool mouseControlEnabled_ = true;
  bool tabPressedLastFrame_ = false;
  double lastX = 0.0;
  double lastY = 0.0;
  bool firstMouse = true;
};
