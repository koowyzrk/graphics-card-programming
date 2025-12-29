#pragma once
#include "core/baseapp.h"
#include "core/camera.h"
#include "core/graphNode.h"
#include "core/skybox.h"
#include <memory>

struct House {
  std::shared_ptr<GraphNode> node;
  std::shared_ptr<GraphNode> rotationNode;
};

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
  SkyBox *skybox_ = nullptr;

  // HousingEstate
  std::vector<House> houses_;
  void createScene();

  Model *generatePlaneModel(float size, std::string textureDir,
                            std::string textureFile);
  Model *generateCubeModel();
  Model *generatePyramidModel();
  //

  // mouse control
  bool mouseControlEnabled_ = true;
  bool tabPressedLastFrame_ = false;
  double lastX = 0.0;
  double lastY = 0.0;
  bool firstMouse = true;
  float cameraSpeed = 10.0f;
};
