#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <stddef.h>

namespace URI
{
  struct URI
  {
    std::string path{};
    std::map<std::string, std::string> queries{};
  };

  // for string delimiter
  std::vector<std::string> split(std::string s, std::string delimiter)
  {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
  }

  URI parse_uri_from_string(const std::string &str)
  {
    URI uri{};

    std::vector<std::string> path_and_query = split(str, "?");

    assert(path_and_query.size() > 0 && path_and_query.size() <= 2);

    uri.path = path_and_query[0];

    if (path_and_query.size() == 2)
    {
      std::string query_str = path_and_query[1];
      std::vector<std::string> queries = split(query_str, "&");
      for (const std::string &query : queries)
      {
        std::vector<std::string> key_val = split(query, "=");
        assert(key_val.size() == 2);
        std::string key = key_val[0];
        std::string val = key_val[1];
        uri.queries[key] = val;
      }
    }

    return uri;
  }
}