#include "mesh.h"
#include "core/shader.h"
#include <cstddef>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
    : vertices_(vertices), indices_(indices), textures_(textures) {
  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int),
               &indices_[0], GL_STATIC_DRAW);

  // vertex position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, normal)));
  // vertex textures
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, texCords)));
  // vertex tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)(offsetof(Vertex, tangent)));

  glBindVertexArray(0);
}

void Mesh::draw(Shader &shader) const {
  // binding textures
  unsigned int diffuse = 1;
  unsigned int specular = 1;
  unsigned int normal = 1;
  unsigned int height = 1;

  // GL_INVALID_OPERATION error generated. <apiElementType> value is invalid;
  // expected GL_INT or GL_UNSIGNED_INT64_NV
  for (int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string type = textures_[i].type;
    if (type == "texture_diffuse")
      number = std::to_string(diffuse++);
    else if (type == "texture_specular")
      number = std::to_string(specular++);
    else if (type == "texture_normal")
      number = std::to_string(normal++);
    else if (type == "texture_height")
      number = std::to_string(height++);

    shader.setUniform((type + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }
  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(drawMode_, static_cast<unsigned int>(indices_.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setDrawMode(GLenum drawMode) { drawMode_ = drawMode; }
