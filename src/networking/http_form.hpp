#pragma once

#include <cassert>
#include <map>
#include <string>

#include "string_utils.hpp"

namespace HTTP {
std::map<std::string, std::string>
parse_http_form_body(const std::string &body) {
  std::map<std::string, std::string> output_map{};
  std::vector<std::string> queries = str_split_unsafe(body, "&");
  for (const std::string &query : queries) {
    std::vector<std::string> key_val = str_split_unsafe(query, "=");
    assert(key_val.size() == 2);
    std::string key = key_val[0];
    std::string val = key_val[1];
    output_map[key] = val;
  }
  return output_map;
}
}