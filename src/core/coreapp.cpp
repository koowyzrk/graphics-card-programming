#include "core/coreapp.h"
#include "core/window.h"
CoreApp::CoreApp() : is_running(false) {}

CoreApp::~CoreApp() {}

void CoreApp::init(unsigned int width, unsigned int height,
                   const std::string &title) {
  window = new Window(width, height, title);
  init_app();
}

void CoreApp::init_app() {};

void CoreApp::run() {
  is_running = true;
  while (is_running) {
    if (window->shouldClose()) {
      is_running = false;
    }
    input();
    update();
    render();
    window->endFrame();
  }
};
