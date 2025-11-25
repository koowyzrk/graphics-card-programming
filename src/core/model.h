#pragma once

#include "core/graphNode.h"
#include "core/mesh.h"
#include "core/shader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <vector>

class Model {
public:
  Model(std::string &path);
  void draw(Shader &shader);

private:
  // std::vector<Mesh> meshes_;
  std::vector<Texture> texturesLoaded_;
  std::string directory_;
  GraphNode root_;

  void loadModel(std::string &path);
  GraphNode processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  void drawNode(const GraphNode &node, Shader &shader,
                const glm::mat4 &parentTransform);

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            const std::string &typeName);
  GLuint textureFromFile(const char *path, const std::string &directory);
};
