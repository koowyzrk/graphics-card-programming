// #include "core/shader.h"
// #include "core/window.h"
// #include "glm/ext/vector_float3.hpp"
// #include "imgui.h"
// #include "imgui_impl/imgui_impl_glfw.h"
// #include "imgui_impl/imgui_impl_opengl3.h"
// #include <stdio.h>

#include "apps/sierpinski/sierpinski.h"
// #include "apps/triangle/triangle.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h> // Initialize with gladLoadGL()

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

// static void glfw_error_callback(int error, const char *description) {
//   fprintf(stderr, "Glfw Error %d: %s\n", error, description);
// }
//
// bool init();
//
// void input();
// void update();
// void render();
//
// void end_frame();
//
// const char *glsl_version = "#version 460";

int main(int, char **) {

  // Triangle app;
  Sierpinski app;
  app.init(1920, 1080, "test");
  app.run();
  return 0;
}

// int main(int, char **) {
//
// int main(int, char **) {
//
//   Triangle tri;
//   tri.init(1920, 1080, "test");
//   tri.run();
//   return 0;
//
//   // if (!init()) {
//   //   spdlog::error("Failed to initialize project!");
//   //   return EXIT_FAILURE;
//   // }
//   // spdlog::info("Initialized project.");
//
//   Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "test");
//
//   // TRIANGLE
//   Shader firstShader("res/shaders/basic.vert", "res/shaders/third.frag");
//   Shader secondShader("res/shaders/basic.vert", "res/shaders/third.frag");
//
//   // float vertices[] = {
//   //     // first triangle
//   //     0.5f, 0.5f,
//   //     0.0f, // top right
//   //     0.5f, -0.5f,
//   //     0.0f, // bottom right
//   //     -0.5f, 0.5f,
//   //     0.0f, // top left
//   //     // second triangle
//   //     0.5f, -0.5f,
//   //     0.0f, // bottom right
//   //     -0.5f, -0.5f,
//   //     0.0f, // bottom left
//   //     -0.5f, 0.5f,
//   //     0.0f // top left
//   // };
//   //   int indicies[] = {
//   //     0, 1, 3, // first triangle
//   //     1, 2, 3  // second triangle
//   // };
//
//   float firstTriangle[] = {
//       // positions      // colors
//       0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
//       1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
//       0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top left
//   };
//   float secondTriangle[] = {
//       // positions      // colors
//       0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
//       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
//       -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top left
//   };
//
//   float texCoords[] = {
//       0.0f, 0.0f, // lower-left corner
//       1.0f, 0.0f, // lower-right corner
//       0.5f, 1.0f  // top-center corner
//   };
//
//   // unsigned int VBO;
//   // glGenBuffers(1, &VBO);
//   GLuint VBOs[2];
//   glGenBuffers(2, VBOs);
//
//   GLuint EBO;
//   glGenBuffers(1, &EBO);
//
//   // GLuint VAO;
//   // glGenVertexArrays(1, &VAO);
//   // glBindVertexArray(VAO);
//   GLuint VAOs[2];
//   glGenVertexArrays(2, VAOs);
//   glBindVertexArray(VAOs[0]);
//
//   // glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
//   // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
// GL_STATIC_DRAW);
//
// glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
// glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle,
//              GL_STATIC_DRAW);
//
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void
// *)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 3, GL_FLOAT,
// GL_FALSE, 6 * sizeof(float),
//                       (void *)(3 * sizeof(float)));
// glEnableVertexAttribArray(1);
//
// glBindVertexArray(0);
// glBindVertexArray(VAOs[1]);
//
// glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
// glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle,
//              GL_STATIC_DRAW);
//
// // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
// // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
// //              GL_STATIC_DRAW);
//
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void
// *)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 3, GL_FLOAT,
// GL_FALSE, 6 * sizeof(float),
//                       (void *)(3 * sizeof(float)));
// glEnableVertexAttribArray(1);
//
// glBindBuffer(GL_ARRAY_BUFFER, 0);
// // uncomment this call to draw in wireframe polygons.
// // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// //
//
// // Main loop
// while (!window.shouldClose()) {
//   // Process I/O operations here
//   input();
//   // Update game objects' state here
//   update();
//   // OpenGL rendering code here
//   render();
//
//   // TRIANGLE
//   // draw our first triangle
//   firstShader.use();
//   glBindVertexArray(VAOs[0]);
//   glDrawArrays(GL_TRIANGLE_FAN, 0, 3);
//
//   glBindVertexArray(0);
//   secondShader.use();
//
//   // uniform
//   float timeValue = glfwGetTime();
//   float greenValue = sin(timeValue) / 2.0f + 0.5f;
//   secondShader.setUniform("xOffset", 0.0f);
//   secondShader.setUniform("ourColor", glm::vec3(0.0f, greenValue, 0.0f));
//
//   glBindVertexArray(VAOs[1]);
//   glDrawArrays(GL_TRIANGLES, 0, 3);
//   // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//   // glBindVertexArray(0); // no need to unbind it every time
//   //
//
//   // End frame and swap buffers (double buffering)
//   window.endFrame();
// }
//
// glDeleteVertexArrays(2, VAOs);
// glDeleteBuffers(2, VBOs);
//
// // firstShader.deleteProgram();
// // secondShader.deleteProgram();
//
// // glfwDestroyWindow(window);
// // glfwTerminate();
// return 0;
// }
//
// void input() {
//   // I/O ops go here
// }
//
// void update() {
//   // Update game objects' state here
// }
//
// void render() {
//   // OpenGL Rendering code goes here
//   glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//   glClear(GL_COLOR_BUFFER_BIT);
// }
