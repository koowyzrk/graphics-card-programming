#include <iostream>
#include <stb_image.h>

#include "GLFW/glfw3.h"
#include "core/shader.h"
#include "core/utils.h"
#include "triangle.h"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

Triangle::Triangle() {};
Triangle::~Triangle() {};

void Triangle::init_app() {

  std::string dir = "src/apps/triangle/res/";
  firstShader =
      new Shader(dir + "shaders/basic.vert", dir + "shaders/basic.frag");
  secondShader =
      new Shader(dir + "shaders/basic.vert", dir + "shaders/basic.frag");

  float vertices[] = {
      // first triangle
      // positions        // colors         //texture_cords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  int indicies[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  // SETTING UP VAO,VBO and EBO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // GENERATING TEXTURE
  // unsigned int texture, texture2;
  // texture 1
  // ---------
  glGenTextures(2, textures);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // activate texture unit
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

  // texture 2
  // ---------
  // glGenTextures(1, texture2);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  // set the texture wrapping parameters
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glActiveTexture(GL_TEXTURE1);
  stbi_set_flip_vertically_on_load(true);
  data = Utils::loadTexture(dir, "textures/awesomeface.png", &width, &height,
                            &nrChannels);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
  //

  // tell opengl for each sampler to which texture unit it belongs to (only has
  // to be done once)
  // -------------------------------------------------------------------------------------------
  firstShader->use();
  firstShader->setUniform("ourTexture", 0);
  firstShader->setUniform("addTexture", 1);

  // // transormations
  // glm::mat4 trans = glm::mat4(1.0f);
  // // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0,
  // 0.0, 1.0));
  // // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
  // trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f,
  // 0.0f, 1.0f)); firstShader->setUniform("transform", trans);
};

void Triangle::input() {
  ImGui::Begin("Controls");
  ImGui::End();
};

void Triangle::update() {};

void Triangle::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // bind textures on corresponding texture units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);

  firstShader->use();

  // transormations
  // rotating using uniform
  // going to 3D
  // VClip = MProjection * MView * MModel * VLocal
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 view = glm::mat4(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

  glm::mat4 projection;
  projection =
      glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

  firstShader->setUniform("model", model);
  firstShader->setUniform("view", view);
  firstShader->setUniform("projection", projection);

  glm::mat4 trans = glm::mat4(1.0f);
  trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
  trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
  firstShader->setUniform("transform", trans);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0); // no need to unbind it every time
  // End frame and swap buffers (double buffering) we do it in baseapp
  // window->endFrame();
};
