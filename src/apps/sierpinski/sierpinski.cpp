#include "sierpinski.h"
#include "glm/exponential.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

Sierpinski::Sierpinski() {};
Sierpinski::~Sierpinski() {};

void Sierpinski::init_app() {
  std::string dir = "src/apps/sierpinski/res/";
  shader = new Shader(dir + "shaders/basic.vert", dir + "shaders/basic.frag");

  glm::vec3 A(-0.5f, 0.0f, 0.0f);
  glm::vec3 B(0.0f, 0.0f, -(0.5f * glm::sqrt(3.0f)) / 2.0f);
  glm::vec3 C(0.5f, 0.0f, 0.0f);
  glm::vec3 D(0.0f, (0.5f * glm::sqrt(6.0f)) / 3.0f,
              -(0.5f * glm::sqrt(3.0f)) / 6.0f);

  generatePyramid(A, B, C, D, recursionLevel);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

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
};
void Sierpinski::input() {
  ImGui::Begin("Controls");
  ImGui::SliderInt("Recursion Level", &recursionLevel, 0, 5);
  ImGui::ColorEdit3("Color", glm::value_ptr(fracColor));
  ImGui::SliderFloat("Rotate X", &rotationX, 0.0f, 360.0f);
  ImGui::SliderFloat("Rotate Y", &rotationY, 0.0f, 360.0f);
  ImGui::End();
};
void Sierpinski::update() {
  if (recursionLevel != lastRecursionLevel) {
    vertices.clear();

    glm::vec3 A(-0.5f, 0.0f, 0.0f);
    glm::vec3 B(0.0f, 0.0f, -(0.5f * glm::sqrt(3.0f)) / 2.0f);
    glm::vec3 C(0.5f, 0.0f, 0.0f);
    glm::vec3 D(0.0f, (0.5f * glm::sqrt(6.0f)) / 3.0f,
                -(0.5f * glm::sqrt(3.0f)) / 6.0f);

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
  glClear(GL_COLOR_BUFFER_BIT);

  // bind textures on corresponding texture units
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  shader->use();
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::rotate(transform, glm::radians(rotationX),
                          glm::vec3(1.0f, 0.0f, 0.0f));
  transform = glm::rotate(transform, glm::radians(rotationY),
                          glm::vec3(0.0f, 1.0f, 0.0f));
  shader->setUniform("transform", transform);

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

  glBindVertexArray(0); // no need to unbind it every time
  // End frame and swap buffers (double buffering)
  window->endFrame();
};

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
  glm::vec2 t1(0.0f, 0.0f);
  glm::vec2 t2(1.0f, 0.0f);
  glm::vec2 t3(0.5f, 1.0f);
  vertices.insert(vertices.end(), {a.x, a.y, a.z, t1.x, t1.y, b.x, b.y, b.z,
                                   t2.x, t2.y, c.x, c.y, c.z, t3.x, t3.y});
}
