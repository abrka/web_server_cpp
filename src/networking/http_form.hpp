#pragma once

#include <cassert>

#include "string_utils.hpp"
#include "url_decoder.hpp"

namespace HTTP {
std::map<std::string, std::string>
parse_http_form_body(const std::string &body) {
  std::map<std::string, std::string> output_map{};
  std::vector<std::string> queries = str_split_unsafe(body, "&");
  for (const std::string &query : queries) {
    std::vector<std::string> key_val = str_split_unsafe(query, "=");
    assert(key_val.size() == 2);
    std::string key = key_val[0];
    std::string key_decoded = url_decode(key);
    std::string val = key_val[1];
    std::string val_decoded = url_decode(val);
    output_map[key_decoded] = val_decoded;
  }
  return output_map;
}
} // namespace HTTP