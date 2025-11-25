#include "model.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include "core/graphNode.h"
#include "core/mesh.h"
#include "core/shader.h"
#include "core/utils.h"
#include "glm/ext/vector_float2.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

static glm::mat4 aiToGlm(const aiMatrix4x4 &m) {
  glm::mat4 out;
  out[0][0] = m.a1;
  out[1][0] = m.a2;
  out[2][0] = m.a3;
  out[3][0] = m.a4;
  out[0][1] = m.b1;
  out[1][1] = m.b2;
  out[2][1] = m.b3;
  out[3][1] = m.b4;
  out[0][2] = m.c1;
  out[1][2] = m.c2;
  out[2][2] = m.c3;
  out[3][2] = m.c4;
  out[0][3] = m.d1;
  out[1][3] = m.d2;
  out[2][3] = m.d3;
  out[3][3] = m.d4;
  return out;
}

Model::Model(std::string &path) { loadModel(path); }

void Model::draw(Shader &shader) { drawNode(root_, shader, glm::mat4(1.0f)); }

void Model::drawNode(const GraphNode &node, Shader &shader,
                     const glm::mat4 &parentTransform) {
  glm::mat4 global = parentTransform * node.getTransform();
  shader.setUniform("model", global);

  for (const Mesh &mesh : node.getMeshes()) {
    mesh.draw(shader);
  }

  for (const GraphNode &child : node.getChildren()) {
    drawNode(child, shader, global);
  }
}

void Model::loadModel(std::string &path) {
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
  directory_ = path.substr(0, path.find_last_of('/'));
  root_ = processNode(scene->mRootNode, scene);
}

GraphNode Model::processNode(aiNode *node, const aiScene *scene) {
  glm::mat4 transform = aiToGlm(node->mTransformation);
  GraphNode graphNode(transform);
  // Process meshes at this node
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    graphNode.addMesh(processMesh(mesh, scene));
  }
  // Recursively build child nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    graphNode.addChild(processNode(node->mChildren[i], scene));
  }
  return graphNode;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
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
  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (GLuint j = 0; j < face.mNumIndices; j++) {
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
      loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  std::vector<Texture> heightMaps =
      loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat,
                                                 aiTextureType type,
                                                 const std::string &typeName) {
  std::vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
    bool skip = false;
    aiString str;
    mat->GetTexture(type, i, &str);

    for (GLuint j = 0; j < texturesLoaded_.size(); j++) {
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
  std::string filename = std::string(path);

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
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB,
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
