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

std::string get_local_path_from_uri_path(const std::string &uri) {

  std::string path = uri;
  path.erase(0, 1);
  return path;
}

std::string get_filepath_from_uri(const std::string &uri_path) {
  if (uri_path == "/") {
    return "index.html";
  } else {
    return get_local_path_from_uri_path(uri_path);
  }
}