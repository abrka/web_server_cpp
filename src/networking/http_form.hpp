#pragma once

#include <cassert>
#include <map>
#include <string>
#include <regex>

#include "string_utils.hpp"

namespace HTTP {

// encoding : decoding
static std::map<std::string, std::string> url_decode_map{
  {R"(%3A)",":"},
  {R"(%3A)",":"},
  {R"(%2F)","/"},
  {R"(%3F)","?"},
  {R"(%23)","#"},
  {R"(%5B)","["},
  {R"(%5D)","]"},
  {R"(%40)","@"},
  {R"(%21)","!"},
  {R"(%24)","$"},
  {R"(%26)","&"},
  {R"(%27)","'"},
  {R"(%28)","("},
  {R"(%29)",")"},
  {R"(%2A)","*"},
  {R"(%2B)","+"},
  {R"(%2C)",","},
  {R"(%3B)",";"},
  {R"(%3D)","="},
  {R"(%25)","%"},
  {R"(%20)"," "},
  {R"(\+)"," "}
};

std::string url_decode(const std::string& str){
  std::string output = str;
  for(const auto&[encoding,decoding] : url_decode_map){
    output = std::regex_replace(output, std::regex(encoding), decoding);
  }
  return output;
}

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