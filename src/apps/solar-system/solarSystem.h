#pragma once
#include "core/baseapp.h"

class SolarSystem : public BaseApp {
public:
  SolarSystem();
  ~SolarSystem() override;

protected:
  void init_app() override;
  void input() override;
  void update() override;
  void render() override;

private:
};
