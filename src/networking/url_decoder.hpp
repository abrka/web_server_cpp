#pragma once

#include <map>
#include <string>
#include <regex>

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