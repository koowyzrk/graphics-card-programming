#include "apps/housing-estate/housingEstate.h"
#include "apps/sierpinski/sierpinski.h"
#include "apps/solar-system/solarSystem.h"
#include "apps/triangle/triangle.h"
#include <iostream>

int main(int, char **) {

  int choice = 0;

  std::cout << "=============================\n";
  std::cout << "   Select Project to Run\n";
  std::cout << "=============================\n";
  std::cout << "1. Triangle\n";
  std::cout << "2. Sierpinski\n";
  std::cout << "3. Solar System\n";
  std::cout << "4. Housing Estate\n";
  std::cout << "-----------------------------\n";
  std::cout << "Enter choice (1-4): ";
  std::cin >> choice;

  if (std::cin.fail() ||
      (choice != 1 && choice != 2 && choice != 3 && choice != 4)) {
    std::cerr << "Invalid choice. Exiting.\n";
    return -1;
  }

  BaseApp *app = nullptr;

  switch (choice) {
  case 1:
    std::cout << "Launching Triangle project...\n";
    app = new Triangle();
    break;
  case 2:
    std::cout << "Launching Sierpinski project...\n";
    app = new Sierpinski();
    break;
  case 3:
    std::cout << "Launching Solar System project...\n";
    app = new SolarSystem();
    break;
  case 4:
    std::cout << "Launching Housing Estate project...\n";
    app = new HousingEstate();
    break;
  }

  if (!app) {
    std::cerr << "Failed to create application.\n";
    return -1;
  }

  app->init(1920, 1080, "OpenGL Project");
  app->run();

  delete app;
  return 0;
}
