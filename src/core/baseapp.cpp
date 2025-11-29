#include "core/baseapp.h"
#include "core/window.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
const char *glsl_version = "#version 460";

BaseApp::BaseApp() : is_running(false) {}
BaseApp::~BaseApp() {}

void BaseApp::init(unsigned int width, unsigned int height,
                   const std::string &title) {
  window = new Window(width, height, title);
  gui = new Gui(window->getWindow(), glsl_version);
  init_app();
}
void BaseApp::run() {
  is_running = true;
  double lastTime = glfwGetTime();
  while (is_running) {
    if (window->shouldClose()) {
      is_running = false;
    }

    // calculating deltaTime
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    gui->begin();
    input();
    update();
    render();
    render_gui();
    gui->end();
    window->endFrame();
  }
};
