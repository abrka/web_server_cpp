#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

int read_str_from_file(const std::string &filepath, std::string &contents) {
  std::ifstream t(filepath);
  if (t.fail()) {
    return -1;
  }
  std::stringstream buffer;
  buffer << t.rdbuf();
  contents = buffer.str();
  return 0;
}

std::string get_extension_of_file(const std::string file) {
  return std::filesystem::path(file).extension().string();
}

std::filesystem::path get_filepath_from_uri_path(const std::string &uri_path) {
  std::string path = uri_path;
  path.erase(0, 1);
  return std::filesystem::path(path);
}