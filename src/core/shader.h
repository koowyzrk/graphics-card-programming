#pragma once
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <map>

class Shader {
public:
  Shader();
  Shader(const std::filesystem::path &vertex_shader_filepath,
         const std::filesystem::path &fragment_shader_filepath);
  ~Shader();
  void deleteProgram() const;
  void use() const;

  void setUniform(const std::string &uniform_name, float value);
  void setUniform(const std::string &uniform_name, int value);
  void setUniform(const std::string &uniform_name, GLuint value);
  void setUniform(const std::string &uniformName, const glm::vec3 &vector);
  void setUniform(const std::string &uniformName, const glm::vec4 &vector);
  void setUniform(const std::string &uniformName, const glm::mat4 &matrices);

private:
  void addShader(const std::filesystem::path &shader_filepath, GLuint type);
  bool link();
  bool getUniformLocation(const std::string &uniform_name);

  std::map<std::string, GLint> uniforms_locations;

  GLuint program_id;
  bool is_linked;
};
