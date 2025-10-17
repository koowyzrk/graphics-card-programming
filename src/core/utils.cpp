#include "utils.h"
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

std::string Utils::getFileContent(const std::filesystem::path &filename) {
  if (filename.empty()) {
    return {};
  }

  std::filesystem::path filepath = std::filesystem::current_path() / filename;
  std::ifstream infile(filepath);

  if (!infile) {
    fprintf(stderr, "Could not open file %s\n", filepath.string().c_str());
    infile.close();
    return {};
  }

  std::ostringstream contents;
  contents << infile.rdbuf();

  return contents.str();
};
