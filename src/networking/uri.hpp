#pragma once

#include <cassert>
#include <map>
#include <stddef.h>
#include <string>
#include <vector>

#include "key_val_parser.hpp"

namespace URI {
struct URI {
  std::string path{};
  std::map<std::string, std::string> queries{};
};

URI parse_uri_from_string(const std::string &str) {
  URI uri{};

  std::vector<std::string> path_and_query = str_split_unsafe(str, "?");

  assert(path_and_query.size() > 0 && path_and_query.size() <= 2);

  uri.path = path_and_query[0];

  if (path_and_query.size() == 2) {
    std::string query_str = path_and_query[1];
    uri.queries = HTTP::parse_key_val_string(query_str);
  }

  return uri;
}
}