#include "shader.h"
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include <cstdio>

Shader::Shader() : program_id(0), is_linked(false) {
  program_id = glCreateProgram();
  if (program_id == 0) {
    fprintf(stderr, "Error while creating program object.\n");
  }
};

Shader::Shader(const std::filesystem::path &vertex_shader_filepath,
               const std::filesystem::path &fragment_shader_filepath)
    : Shader() {
  addShader(vertex_shader_filepath, GL_VERTEX_SHADER);
  addShader(fragment_shader_filepath, GL_FRAGMENT_SHADER);
  link();
};

Shader::~Shader() {}

void Shader::deleteProgram() const {
  if (program_id != 0 && glIsProgram(program_id)) {
    glDeleteProgram(program_id);
  }
}

void Shader::addShader(const std::filesystem::path &shader_filepath,
                       GLuint type) {
  if (program_id == 0) {
    return;
  }
  if (shader_filepath.empty()) {
    fprintf(stderr, "Error: Shader's file name can't be empty.\n");
    return;
  }

  GLuint shader = glCreateShader(type);
  if (shader == 0) {
    fprintf(stderr, "Error while creating %s.\n", shader_filepath.c_str());
    return;
  }

  std::string shaderCode = Utils::getFileContent(shader_filepath);
  const char *shaderSource = shaderCode.c_str();
  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);

  GLint result;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    fprintf(stderr, "\n%s compilation failed!\n",
            shader_filepath.string().c_str());

    GLint logLen;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

    if (logLen > 0) {
      char *log = static_cast<char *>(malloc(logLen));
      GLsizei written;
      glGetShaderInfoLog(shader, logLen, &written, log);
      fprintf(stderr, "Shader log: \n%s", log);
      free(log);
    }
    getchar();
    return;
  }

  glAttachShader(program_id, shader);
  glDeleteShader(shader);
};

bool Shader::link() {
  glLinkProgram(program_id);

  GLint status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    fprintf(stderr, "Failed to link shader program!\n");

    GLint logLen;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLen);

    if (logLen > 0) {
      char *log = (char *)malloc(logLen);
      GLsizei written;
      glGetProgramInfoLog(program_id, logLen, &written, log);
      fprintf(stderr, "Program log: \n%s", log);
      free(log);
    }
  } else {
    is_linked = true;
    // addAllSubroutines();
  }

  return is_linked;
}

void Shader::use() const {
  if (!is_linked) {
    fprintf(stderr, "Warning: Tried to use an unlinked shader program!\n");
    return;
  }
  glUseProgram(program_id);
}

bool Shader::getUniformLocation(const std::string &uniform_name) {
  GLint uniform_location =
      glGetUniformLocation(program_id, uniform_name.c_str());

  if (uniform_location != -1) {
    uniforms_locations[uniform_name] = uniform_location;
    return true;
  }
  return false;
}

void Shader::setUniform(const std::string &uniformName, float value) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniform1f(program_id, uniforms_locations[uniformName], value);
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniform1f(program_id, uniforms_locations[uniformName], value);
    }
  }
}

void Shader::setUniform(const std::string &uniformName, int value) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniform1i(program_id, uniforms_locations[uniformName], value);
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniform1i(program_id, uniforms_locations[uniformName], value);
    }
  }
}

void Shader::setUniform(const std::string &uniformName, GLuint value) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniform1ui(program_id, uniforms_locations.at(uniformName), value);
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniform1ui(program_id, uniforms_locations[uniformName], value);
    }
  }
}

void Shader::setUniform(const std::string &uniformName,
                        const glm::vec3 &vector) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniform3fv(program_id, uniforms_locations[uniformName], 1,
                        glm::value_ptr(vector));
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniform3fv(program_id, uniforms_locations[uniformName], 1,
                          glm::value_ptr(vector));
    }
  }
}

void Shader::setUniform(const std::string &uniformName,
                        const glm::vec4 &vector) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniform4fv(program_id, uniforms_locations[uniformName], 1,
                        glm::value_ptr(vector));
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniform4fv(program_id, uniforms_locations[uniformName], 1,
                          glm::value_ptr(vector));
    }
  }
}

void Shader::setUniform(const std::string &uniformName,
                        const glm::mat4 &matrix) {
  if (uniforms_locations.count(uniformName)) {
    glProgramUniformMatrix4fv(program_id, uniforms_locations[uniformName], 1,
                              GL_FALSE, glm::value_ptr(matrix));
  } else {
    if (getUniformLocation(uniformName)) {
      glProgramUniformMatrix4fv(program_id, uniforms_locations[uniformName], 1,
                                GL_FALSE, glm::value_ptr(matrix));
    }
  }
}
