#pragma once
#include "core/shader.h"
#include "glm/ext/matrix_float4x4.hpp"
#include <string>
#include <vector>

class SkyBox {
public:
  SkyBox(const std::string &directory, const std::vector<std::string> &faces,
         const std::string &vertPath, const std::string &fragPath);
  ~SkyBox();

  void draw(const glm::mat4 &view, const glm::mat4 &projection);
  unsigned int getTextureID() { return textureID_; }

private:
  unsigned int vao_, vbo_;
  unsigned int textureID_;
  Shader *shader_;

  void setup();
  unsigned int loadCubeMap(const std::string &directory,
                           const std::vector<std::string> &faces);
};
