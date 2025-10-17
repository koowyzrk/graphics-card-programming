#include "core/coreapp.h"
#include "core/shader.h"

class Triangle : public CoreApp {
public:
  Triangle();
  ~Triangle() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
  Shader *firstShader = nullptr;
  Shader *secondShader = nullptr;
  GLuint VAOs[2]{};
  GLuint VBOs[2]{};
  GLuint EBO = 0;
};
