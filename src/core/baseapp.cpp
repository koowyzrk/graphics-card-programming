#include "core/baseapp.h"
#include "core/window.h"
BaseApp::BaseApp() : is_running(false) {}

BaseApp::~BaseApp() {}

void BaseApp::init(unsigned int width, unsigned int height,
                   const std::string &title) {
  window = new Window(width, height, title);
  init_app();
}

void BaseApp::init_app() {};

void BaseApp::run() {
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
