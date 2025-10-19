#pragma once
#include "imgui_impl/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <string>

constexpr int32_t WINDOW_WIDTH = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 6;

class Window {
public:
  Window(unsigned int width, unsigned int height, const std::string &title);
  ~Window();

  static GLFWwindow *getWindow();

  static int shouldClose();

  void endFrame();
  void setVSync(bool enabled);

private:
  static GLFWwindow *window;
  static std::string title;
  static glm::ivec2 window_size;

  static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
  }
};
