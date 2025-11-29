#include "sierpinski.h"
#include "GLFW/glfw3.h"
#include "core/camera.h"
#include "core/utils.h"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"
#include <iostream>

Sierpinski::Sierpinski() {};
Sierpinski::~Sierpinski() {};

void Sierpinski::init_app() {
  std::string dir = "src/apps/sierpinski/res/";
  shader = new Shader(dir + "shaders/basic.vert", dir + "shaders/basic.frag");
  camera = new Camera(Camera::Perspective(glm::vec3(0.0f, 0.0f, 3.0f),
                                          glm::radians(45.0f),
                                          1920.0f / 1080.0f, 0.1f, 100.0f));

  // for mouse
  glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  generatePyramid(A, B, C, D, recursionLevel);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  // only with color and texture attribute
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void
  // *)0); glEnableVertexAttribArray(0); glVertexAttribPointer(1, 2, GL_FLOAT,
  // GL_FALSE, 8 * sizeof(float),
  //                       (void *)(3 * sizeof(float)));
  // glEnableVertexAttribArray(1);
  // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
  //                       (void *)(5 * sizeof(float)));
  // glEnableVertexAttribArray(2);

  // only with texture atribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE0);
  int width, height, nrChannels;
  unsigned char *data = Utils::loadTexture(dir, "textures/stone.jpg", &width,
                                           &height, &nrChannels);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  // tell opengl for each sampler to which texture unit it belongs to (only has
  // to be done once)
  // -------------------------------------------------------------------------------------------
  shader->use();
  shader->setUniform("texture1", 0);

  glEnable(GL_DEPTH_TEST);
};

void Sierpinski::input() {
  GLFWwindow *glfwWin = window->getWindow();
  if (glfwGetKey(glfwWin, GLFW_KEY_W) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement ::FORWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_S) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement ::BACKWARD, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_D) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement ::RIGHT, deltaTime);
  if (glfwGetKey(glfwWin, GLFW_KEY_A) == GLFW_PRESS)
    camera->processKeyboard(CameraMovement ::LEFT, deltaTime);

  double mouseX, mouseY;
  glfwGetCursorPos(glfwWin, &mouseX, &mouseY);
  if (firstMouse) {
    lastX = mouseX;
    lastY = mouseY;
    firstMouse = false;
  }
  float xOffset = mouseX - lastX;
  float yOffset = lastY - mouseY;
  lastX = mouseX;
  lastY = mouseY;
  camera->processMouseMovement(xOffset, yOffset);
};

void Sierpinski::update() {
  if (recursionLevel != lastRecursionLevel) {
    vertices.clear();

    generatePyramid(A, B, C, D, recursionLevel);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    lastRecursionLevel = recursionLevel;
  }
};

void Sierpinski::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // bind textures on corresponding texture units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  // calculating delta time
  // float currentFrame = glfwGetTime();
  // deltaTime = currentFrame - lastFrame;
  // lastFrame = currentFrame;
  //

  shader->use();
  // model
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::rotate(transform, glm::radians(rotationX),
                          glm::vec3(1.0f, 0.0f, 0.0f));
  transform = glm::rotate(transform, glm::radians(rotationY),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  shader->setUniform("transform", transform);

  // glm::mat4 view = glm::mat4(1.0f);
  // view = glm::translate(view, glm::vec3(0.0f, 0.0f, cameraPos));
  shader->setUniform("view", camera->getViewMatrix());

  // glm::mat4 projection =
  //     glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
  shader->setUniform("projection", camera->getProjection());

  // additional
  shader->setUniform("saturation", saturation);
  double offset = (float)glfwGetTime();
  glm::vec2 texOffset = glm::vec2(offset, offset);
  shader->setUniform("TexOffset", texOffset);
  //

  shader->setUniform("fracColor", fracColor);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

  glBindVertexArray(0);
};

void Sierpinski::render_gui() {
  ImGui::Begin("Controls");
  ImGui::SliderInt("Recursion Level", &recursionLevel, 0, 5);
  ImGui::ColorEdit3("Color", glm::value_ptr(fracColor));
  ImGui::SliderFloat("Rotate X", &rotationX, 0.0f, 360.0f);
  ImGui::SliderFloat("Rotate Y", &rotationY, 0.0f, 360.0f);
  // additional
  ImGui::SliderFloat("Saturation", &saturation, 0.0f, 1.0f);
  //
  ImGui::End();
}

void Sierpinski::generatePyramid(glm::vec3 a, glm::vec3 b, glm::vec3 c,
                                 glm::vec3 d, int depth) {
  if (depth == 0) {
    // add to vector last a,b,c,d
    addTriangle(a, b, c);
    addTriangle(a, b, d);
    addTriangle(a, c, d);
    addTriangle(b, c, d);
    return;
  }
  // dividing the points to make more edges
  glm::vec3 ab = (a + b) * 0.5f;
  glm::vec3 ac = (a + c) * 0.5f;
  glm::vec3 ad = (a + d) * 0.5f;
  glm::vec3 bd = (b + d) * 0.5f;
  glm::vec3 bc = (c + b) * 0.5f;
  glm::vec3 cd = (c + d) * 0.5f;
  // generatePyramid(new a,new b,new d,depth -1 )
  generatePyramid(a, ab, ac, ad, depth - 1);
  generatePyramid(ab, b, bc, bd, depth - 1);
  generatePyramid(ac, bc, c, cd, depth - 1);
  generatePyramid(ad, bd, cd, d, depth - 1);
}

void Sierpinski::addTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
  // defining color atributes for triangle
  // glm::vec3 c1(1.0f, 0.0f, 0.0f);
  // glm::vec3 c2(0.0f, 1.0f, 0.0f);
  // glm::vec3 c3(0.0f, 0.0f, 1.0f);

  // defining textures cords for one triangle
  glm::vec2 t1(0.0f, 0.0f);
  glm::vec2 t2(1.0f, 0.0f);
  glm::vec2 t3(0.5f, 1.0f);

  // only with texture atributes
  vertices.insert(vertices.end(), {a.x, a.y, a.z, t1.x, t1.y, b.x, b.y, b.z,
                                   t2.x, t2.y, c.x, c.y, c.z, t3.x, t3.y});

  // adding extra color atributes
  // vertices.insert(vertices.end(),
  //                 {a.x, a.y, a.z, t1.x, t1.y, c1.x, c1.y, c1.z,
  //                  b.x, b.y, b.z, t2.x, t2.y, c2.x, c2.y, c2.z,
  //                  c.x, c.y, c.z, t3.x, t3.y, c3.x, c3.y, c3.z});
}
