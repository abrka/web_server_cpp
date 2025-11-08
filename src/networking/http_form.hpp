#pragma once

#include <cassert>

#include "string_utils.hpp"
#include "url_decoder.hpp"
#include "key_val_parser.hpp"

namespace HTTP {
std::map<std::string, std::string>
parse_http_urlencoded_form_body(const std::string &body) {
  return parse_key_val_string(body);
}
} // namespace HTTP