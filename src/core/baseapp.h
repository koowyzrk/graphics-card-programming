#include "GLFW/glfw3.h"
#include "core/shader.h"
#include "core/window.h"
#include <string>
class BaseApp {
public:
  BaseApp();
  virtual ~BaseApp();

  virtual void init(unsigned int width, unsigned int height,
                    const std::string &title);
  void run();

protected:
  virtual void init_app() = 0;
  virtual void input() = 0;
  virtual void update() = 0;
  virtual void render() = 0;

  bool is_running = false;
  Window *window = nullptr;
};
