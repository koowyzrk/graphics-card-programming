#include "utils.h"

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

unsigned char *Utils::loadTexture(const std::string &dir,
                                  const std::string &name, int *width,
                                  int *height, int *channels) {
  std::string path = dir + name;
  return stbi_load(path.c_str(), width, height, channels, 0);
}
