#include <cassert>
#include <map>
#include <stddef.h>
#include <string>
#include <vector>

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
    std::vector<std::string> queries = str_split_unsafe(query_str, "&");
    for (const std::string &query : queries) {
      std::vector<std::string> key_val = str_split_unsafe(query, "=");
      assert(key_val.size() == 2);
      std::string key = key_val[0];
      std::string val = key_val[1];
      uri.queries[key] = val;
    }
  }

  return uri;
}
}