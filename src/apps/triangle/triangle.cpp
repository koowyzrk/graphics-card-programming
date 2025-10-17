#include "triangle.h"

Triangle::Triangle() {};
Triangle::~Triangle() {};

void Triangle::init_app() {

  firstShader = new Shader("res/shaders/basic.vert", "res/shaders/third.frag");
  secondShader = new Shader("res/shaders/basic.vert", "res/shaders/third.frag");

  // float vertices[] = {
  //     // first triangle
  //     0.5f, 0.5f,
  //     0.0f, // top right
  //     0.5f, -0.5f,
  //     0.0f, // bottom right
  //     -0.5f, 0.5f,
  //     0.0f, // top left
  //     // second triangle
  //     0.5f, -0.5f,
  //     0.0f, // bottom right
  //     -0.5f, -0.5f,
  //     0.0f, // bottom left
  //     -0.5f, 0.5f,
  //     0.0f // top left
  // };
  //   int indicies[] = {
  //     0, 1, 3, // first triangle
  //     1, 2, 3  // second triangle
  // };

  float firstTriangle[] = {
      // positions      // colors
      0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
      1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
      0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // top left
  };
  float secondTriangle[] = {
      // positions      // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top left
  };

  glGenBuffers(2, VBOs);
  glGenVertexArrays(2, VAOs);

  // GLuint EBO;
  // glGenBuffers(1, &EBO);

  glBindVertexArray(VAOs[0]);
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  glBindVertexArray(VAOs[1]);
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle,
               GL_STATIC_DRAW);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
  //              GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
};

void Triangle::input() {};

void Triangle::update() {};

void Triangle::render() {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  firstShader->use();
  glBindVertexArray(VAOs[0]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

  glBindVertexArray(0);
  secondShader->use();

  // uniform
  float timeValue = glfwGetTime();
  float greenValue = sin(timeValue) / 2.0f + 0.5f;
  secondShader->setUniform("xOffset", 0.0f);
  secondShader->setUniform("ourColor", glm::vec3(0.0f, greenValue, 0.0f));

  glBindVertexArray(VAOs[1]);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // glBindVertexArray(0); // no need to unbind it every time
  // End frame and swap buffers (double buffering)
  window->endFrame();
};
