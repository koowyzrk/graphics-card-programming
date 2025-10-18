#include "core/window.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam);

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
#ifdef _DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
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

#ifdef _DEBUG
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    std::cout << "[OpenGL] Debug context enabled.\n";

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }
#endif
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

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // Ignore non-significant error/warning codes (optional, per LearnOpenGL)
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------\n";
  std::cout << "Debug message (" << id << "): " << message << "\n";

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << "\n";

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << "\n";

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: HIGH";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: MEDIUM";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: LOW";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: NOTIFICATION";
    break;
  }
  std::cout << "\n\n";
}
