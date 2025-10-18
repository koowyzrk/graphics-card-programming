#pragma once
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stb_image.h>

#include <filesystem>
class Utils {
public:
  static std::string getFileContent(const std::filesystem::path &filename);
  static unsigned char *loadTexture(const std::string &dir,
                                    const std::string &name, int *width,
                                    int *height, int *channels);
};
