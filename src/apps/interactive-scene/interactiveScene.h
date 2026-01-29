#pragma once
#include "core/baseapp.h"
#include "core/camera.h"
#include "core/graphNode.h"
#include "core/light.h"
#include "core/shader.h"
#include "core/skybox.h"
#include <climits>
#include <memory>

#define GLM_ENABLE_EXPERIMENTAL

struct HouseScene {
  std::shared_ptr<GraphNode> root;
  std::shared_ptr<GraphNode> wall;
  std::shared_ptr<GraphNode> roof;
};

class InteractiveScene : public BaseApp {
public:
  InteractiveScene();
  ~InteractiveScene() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;
  void render_gui() override;

private:
  std::shared_ptr<GraphNode> rootNode_;
  Shader *shader_ = nullptr;
  Shader *reflectShader_ = nullptr;
  Shader *refractionShader_ = nullptr;
  Camera *camera_ = nullptr;
  SkyBox *skybox_ = nullptr;

  // InteractiveScene
  std::shared_ptr<GraphNode> reflectiveNode_;
  std::shared_ptr<GraphNode> refractionNode_;

  std::unique_ptr<Model> cube_;
  std::unique_ptr<Model> character_;
  //

  // HousingEstate
  int selectedHouseIndex = 0;
  bool bufferUpdate = true;
  bool editHouse = false;

  std::vector<HouseScene> houses_;
  void createScene(int houseCount);

  Model *generatePlaneModel(float size, std::string textureDir,
                            std::string textureFile);
  Model *generateCubeModel(float size, std::string textureDir);
  Model *generatePyramidModel(float size, std::string textureDir,
                              std::string textureFile);
  //

  // lights
  std::shared_ptr<GraphNode> lightsRoot;
  std::vector<std::shared_ptr<LightNode>> lights;
  Light *directional_ = nullptr;
  Light *spotlight1_ = nullptr;
  Light *spotlight2_ = nullptr;
  Light *point_ = nullptr;
  Model *lightVisualModel;
  //

  // mouse control
  bool mouseControlEnabled_ = true;
  bool tabPressedLastFrame_ = false;
  double lastX = 0.0;
  double lastY = 0.0;
  bool firstMouse = true;
  float cameraSpeed = 50.0f;
};
