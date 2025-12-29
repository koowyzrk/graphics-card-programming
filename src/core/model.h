#pragma once

#include "core/mesh.h"
#include "core/shader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

class Model {
public:
  Model();
  Model(const std::string &path);
  void draw(Shader &shader);

  void addMesh(Mesh mesh);

private:
  std::vector<Mesh> meshes_;
  std::vector<Texture> texturesLoaded_;
  std::string directory_;

  void loadModel(const std::string &path);
  void processNode(aiNode *node, const aiScene *scene,
                   const glm::mat4 &parentTransform);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene,
                   const glm::mat4 &transform);

  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            const std::string &typeName);
  GLuint textureFromFile(const char *path, const std::string &directory);
};
