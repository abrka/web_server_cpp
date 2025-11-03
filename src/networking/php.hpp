#pragma once

#include "pipe.hpp"
#include <string>

std::string parse_file_with_php(const std::string &filepath) {
  std::string cmd = "php " + filepath;
  char *output = read_pipe_output(cmd.c_str());
  std::string output_str{output};
  free(output);
  return output_str;
}