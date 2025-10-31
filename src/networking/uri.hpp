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


  URI parse_uri_from_string(const std::string &str)
  {
    URI uri{};

    std::vector<std::string> path_and_query = str_split(str, "?");

    assert(path_and_query.size() > 0 && path_and_query.size() <= 2);

    uri.path = path_and_query[0];

    if (path_and_query.size() == 2)
    {
      std::string query_str = path_and_query[1];
      std::vector<std::string> queries = str_split(query_str, "&");
      for (const std::string &query : queries)
      {
        std::vector<std::string> key_val = str_split(query, "=");
        assert(key_val.size() == 2);
        std::string key = key_val[0];
        std::string val = key_val[1];
        uri.queries[key] = val;
      }
    }

    return uri;
  }
}