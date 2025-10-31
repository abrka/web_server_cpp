#pragma once

#include <map>
#include <string>

namespace HTTP
{
  std::map<std::string, std::string> file_ext_to_mime_type{{".txt", "text/plain"}, {".html", "text/html"}, {".css", "text/css"}, {".js", "text/javascript"}, {".png", "image/png"}, {".webp", "image/webp"}, {".php", "text/html"}, {".ico", "image/x-icon"}};
  std::string get_mime_type(const std::string &file_ext)
  {
    return file_ext_to_mime_type[file_ext];
  }
}
