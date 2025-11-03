#pragma once

#include <string>
#include <vector>

void str_remove_substr(std::string &s, const std::string &substr) {
  s.erase(s.find(substr), substr.size());
}

std::vector<std::string> str_split_unsafe(const std::string &s,
                                          const std::string &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

bool str_split_first(const std::string &s, const std::string &delimiter,
                     std::string &out_1, std::string &out_2) {
  size_t delim_pos = s.find(delimiter);
  if (delim_pos == std::string::npos) {
    return false;
  }
  out_1 = s.substr(0, delim_pos);
  out_2 = s.substr(delim_pos + delimiter.size());
  return true;
}

std::string
str_extract_substr_between_delims_unsafe(const std::string &s,
                                         const std::string &delim_1,
                                         const std::string &delim_2) {
  size_t delim_1_pos = s.find(delim_1);
  size_t delim_2_pos = s.find(delim_2, delim_1_pos + delim_1.length());

  std::string out = s.substr(delim_1_pos + delim_1.length(),
                             delim_2_pos - (delim_1_pos + delim_1.length()));
  return out;
}

bool str_extract_substr_between_delims(const std::string &s,
                                       const std::string &delim_1,
                                       const std::string &delim_2,
                                       std::string &output) {
  size_t delim_1_pos = s.find(delim_1);
  if (delim_1_pos == std::string::npos) {
    return false;
  }
  size_t delim_2_pos = s.find(delim_2, delim_1_pos + delim_1.length());
  if (delim_2_pos == std::string::npos) {
    return false;
  }

  std::string tmp_output{};

  try {
    tmp_output = s.substr(delim_1_pos + delim_1.length(),
                          delim_2_pos - (delim_1_pos + delim_1.length()));
  } catch (const std::exception &e) {
    return false;
  }

  output = tmp_output;
  return true;
}