#include "core/coreapp.h"

class Sierpinski : public CoreApp {
public:
  Sierpinski();
  ~Sierpinski() override;

  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
};
