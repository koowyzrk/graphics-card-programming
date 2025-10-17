#include "core/coreapp.h"

class Sierpinski : public BaseApp {
public:
  Sierpinski();
  ~Sierpinski() override;

  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
};
