#pragma once

#include <string>

namespace HTTP
{
  struct HttpParserStringStream
  {
    int pos{};
    std::string str{};

    std::string get()
    {
      return str.substr(pos);
    }
    // returns true if the substr matches current string
    bool match(const std::string &substr)
    {
      size_t found_pos = str.find(substr, pos);
      return (found_pos == pos);
    }
    // returns the matched string
    std::string seek_until(const std::string &delim)
    {
      int first_occurrence_pos = str.find(delim, pos);
      if (first_occurrence_pos == std::string::npos)
      {
        return "";
      }
      int new_pos = first_occurrence_pos + delim.size();
      size_t num_chars = first_occurrence_pos - pos;
      std::string match = str.substr(pos, num_chars);
      pos = new_pos;
      return match;
    }
  };
}