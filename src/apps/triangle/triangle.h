#pragma once
#include "core/baseapp.h"
#include "core/shader.h"

class Triangle : public BaseApp {
public:
  Triangle();
  ~Triangle() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;
  void render_gui() override;

private:
  Shader *firstShader = nullptr;
  Shader *secondShader = nullptr;
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;
  unsigned int textures[2]{};
};
