#pragma once

#include <filesystem>
class Utils {
public:
  static std::string getFileContent(const std::filesystem::path &filename);
};
