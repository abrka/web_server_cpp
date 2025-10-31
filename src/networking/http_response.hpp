#pragma once

#include <string>
#include <sstream>

namespace HTTP
{
  struct HttpResponse
  {
    int status_code{};
    std::string reason{};
    std::string content_type{};
    std::string body{};
  };

  std::string http_response_to_str(const HttpResponse &http_response)
  {
    std::stringstream stream{};
    stream << "HTTP/1.1 " << http_response.status_code << " " << http_response.reason << "\r\n"
           << "Content-Type: " << http_response.content_type << "\r\n"
           << "Content-Length: " << http_response.body.length() << "\r\n\r\n"
           << http_response.body;
    std::string response = stream.str();
    return response;
  }
}
