#pragma once

#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

class Gui {
public:
  static void init(GLFWwindow *window, const char *glsl_version);
  static void begin();
  static void end();
  static void shutdown();
};
