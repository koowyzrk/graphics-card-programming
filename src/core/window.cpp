#include "core/window.h"

GLFWwindow *Window::window = nullptr;
std::string Window::title = "";
glm::ivec2 Window::window_size = glm::ivec2(0);

Window::Window(unsigned int width, unsigned int height,
               const std::string &title) {
  this->title = title;
  window_size = glm::ivec2(width, height);

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    std::cerr << "ERROR: Could not initialize GLFW." << std::endl;
    exit(EXIT_FAILURE);
  }

  // glfw: initialize and configure
  // ------------------------------
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  window = glfwCreateWindow(window_size.x, window_size.y, title.c_str(),
                            nullptr, nullptr);

  if (!window) {
    std::cerr << "ERROR: Could not create window and OpenGL context."
              << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  setVSync(false);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "ERROR: Could not initialize GLAD." << std::endl;
    exit(EXIT_FAILURE);
  }

  // /* Set the viewport */
  // glfwGetWindowPos(window, &m_window_pos.x, &m_window_pos.y);
  // glfwGetFramebufferSize(window, &m_viewport_size.x, &m_viewport_size.y);
  //
  // glViewport(0, 0, m_viewport_size.x, m_viewport_size.y);
  // setViewportMatrix(m_viewport_size.x, m_viewport_size.y);
  //
  // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //
  // /* Init Input & GUI */
  // Input::init(m_window);
  // GUI::init(m_window);
}

Window::~Window() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
}

int Window::shouldClose() { return glfwWindowShouldClose(window); }

void Window::endFrame() {
  glfwPollEvents();
  glfwMakeContextCurrent(window);
  glfwSwapBuffers(window);
}

void Window::setVSync(bool enabled) { glfwSwapInterval(enabled ? 1 : 0); }
