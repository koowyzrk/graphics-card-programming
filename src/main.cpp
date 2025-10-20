#include "apps/sierpinski/sierpinski.h"
#include "apps/triangle/triangle.h"
#include <iostream>

int main(int, char **) {

  int choice = 2;

  std::cout << "=============================\n";
  std::cout << "   Select Project to Run\n";
  std::cout << "=============================\n";
  std::cout << "1. Triangle\n";
  std::cout << "2. Sierpinski\n";
  std::cout << "-----------------------------\n";
  std::cout << "Enter choice (1-2): ";
  // std::cin >> choice;
  //
  // if (std::cin.fail() || (choice != 1 && choice != 2)) {
  //   std::cerr << "Invalid choice. Exiting.\n";
  //   return -1;
  // }

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
