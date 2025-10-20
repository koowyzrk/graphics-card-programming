#include "core/window.h"
#include "core/debug.h"

/**
 * @brief Constructor for the Window class.
 * initializes GLFW,
 * creates an OpenGL window,
 * sets up the OpenGL context,
 * loads OpenGL function pointers using GLAD.
 *
 * @param width The desired width of the window in pixels.
 * @param height The desired height of the window in pixels.
 * @param title The title string for the window.
 */
Window::Window(unsigned int width, unsigned int height,
               const std::string &title)
    : title_(title), window_size_(width, height) {

  glfwSetErrorCallback(glfw_error_callback);

  // glfw: initialize
  // ------------------------------
  if (!glfwInit()) {
    std::cerr << "ERROR: Could not initialize GLFW." << std::endl;
    exit(EXIT_FAILURE);
  }
  // glfw: configure window hints
  // --------------------------------------------------------
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
  // set OpenGL profile to Core (OpenGL without deprecated functions)
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifdef _DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  // glfw: window creation
  // --------------------
  window = glfwCreateWindow(window_size_.x, window_size_.y, title.c_str(),
                            nullptr, nullptr);
  if (!window) {
    std::cerr << "ERROR: Could not create window and OpenGL context."
              << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  // make created window the current context on OpenGL
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

/**
 * @brief Destructor for the Window class.
 * Cleans up GLFW resources, destroying the window and
 * terminating the GLFW library.
 */
Window::~Window() {
  if (window) {
    glfwDestroyWindow(window);
    window = nullptr;
  }
  glfwTerminate();
}

GLFWwindow *Window::getWindow() { return window; }
glm::ivec2 Window::getSize() { return window_size_; }

void Window::setVSync(bool enabled) { glfwSwapInterval(enabled ? 1 : 0); }

void Window::endFrame() {
  glfwPollEvents(); // Process all pending GLFW events (input, window resize,
                    // etc.)
  glfwMakeContextCurrent(
      window); // Ensure this window's OpenGL context is active
  glfwSwapBuffers(
      window); // Swap front and back buffers to show the rendered frame
}
