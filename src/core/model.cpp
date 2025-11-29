#include "model.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include "core/mesh.h"
#include "core/shader.h"
#include "core/utils.h"
#include "glm/ext/vector_float2.hpp"
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

Model::Model() {}

Model::Model(const std::string &path) { loadModel(path); }

void Model::draw(Shader &shader) {
  for (unsigned int i = 0; i < meshes_.size(); i++) {
    meshes_[i].draw(shader);
  }
}

void Model::addMesh(Mesh mesh) { meshes_.push_back(mesh); }

void Model::loadModel(const std::string &path) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) // if is Not Zero
  {
    std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    exit(EXIT_FAILURE);
  }

  std::cout << "Model loaded successfully: " << scene->mNumMeshes << " meshes, "
            << scene->mNumMaterials << " materials" << std::endl;

  directory_ = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);

  std::cout << "Processed model: " << meshes_.size() << " meshes created"
            << std::endl;
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;

    // position
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    // normals
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal = vector;
    }

    // texture cords
    if (mesh->HasTextureCoords(0)) {
      glm::vec2 texVec;
      texVec.x = mesh->mTextureCoords[0][i].x;
      texVec.y = mesh->mTextureCoords[0][i].y;
      vertex.texCords = texVec;

      if (mesh->HasTangentsAndBitangents()) {
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;
      }
    } else {
      vertex.texCords = glm::vec2(0.0, 0.0);
    }

    vertices.push_back(vertex);
  }

  // indicies
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  // process materials (textures)
  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<Texture> diffuseMaps =
      loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<Texture> specularMaps = loadMaterialTextures(
      material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  std::vector<Texture> normalMaps =
      loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  std::vector<Texture> heightMaps =
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 const std::string &typeName) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    bool skip = false;
    aiString str;
    mat->GetTexture(type, i, &str);

    for (unsigned int j = 0; j < texturesLoaded_.size(); j++) {
      if (std::strcmp(texturesLoaded_[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(texturesLoaded_[j]);
        skip = true;
        break;
      }
    }

    if (!skip) {
      Texture texture;
      texture.id = textureFromFile(str.C_Str(), directory_);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);

      texturesLoaded_.push_back(
          texture); // ensure if there is not any duplicates
    }
  }

  return textures;
}

GLuint Model::textureFromFile(const char *path, const std::string &directory) {
  std::string filename = "/" + std::string(path);

  GLuint textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  unsigned char *data =
      Utils::loadTexture(directory, filename, &width, &height, &nrChannels);

  if (data) {
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  return textureID;
}
