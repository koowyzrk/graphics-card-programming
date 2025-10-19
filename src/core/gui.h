#pragma once

#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

class Gui {
public:
  Gui(GLFWwindow *window, const char *glsl_version);
  ~Gui();

  void begin();
  void end();

private:
  GLFWwindow *window;
  const char *glsl_version;
};
