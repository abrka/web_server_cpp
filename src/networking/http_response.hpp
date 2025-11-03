#pragma once

#include <string>
#include <sstream>
#include <map>

namespace HTTP
{
  struct HttpResponse
  {
    int status_code{};
    std::string reason{};
    std::map<std::string, std::string> headers{};
    std::string body{};
  };

  std::string http_response_to_str(const HttpResponse &http_response)
  {
    std::stringstream stream{};
    stream << "HTTP/1.1 " << http_response.status_code << " " << http_response.reason << "\r\n";
    for (const auto &[key, value] : http_response.headers)
    {
      stream << key << ": " << value << "\r\n";
    }
    stream << "Content-Length: " << http_response.body.length() << "\r\n\r\n";
    stream << http_response.body;
    std::string response = stream.str();
    return response;
  }
}
