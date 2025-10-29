#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <sstream>
#include <map>

int read_str_from_file(const std::string &filepath, std::string &contents)
{
  std::ifstream t(filepath);
  if (t.fail())
  {
    return -1;
  }
  std::stringstream buffer;
  buffer << t.rdbuf();
  contents = buffer.str();
  return 0;
}

namespace HTTP
{
  struct HttpRequestHeader
  {
    std::string key{};
    std::string val{};
  };
  struct HttpRequest
  {
    std::string method{};
    std::string uri{};
    std::vector<HttpRequestHeader> headers{};
    std::string body{};
  };
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


  HttpRequest parse_http_req_str(const std::string &http_req_str)
  {
    enum class PossibleStates
    {

      REQ_TYPE,
      URI,
      HEADER_KEY,
      HEADER_VAL,
      NEXT_HEADER,
      BODY,
      ERROR = 0
    };
    PossibleStates state = PossibleStates::REQ_TYPE;
    PossibleStates next_state = PossibleStates::REQ_TYPE;
    HttpRequest http_req{};
    for (size_t i = 0; i < http_req_str.length(); i++)
    {
      std::string c = std::string{http_req_str[i]};
      if (state == PossibleStates::REQ_TYPE)
      {
        if (c == " ")
        {
          next_state = PossibleStates::URI;
          // i += 1;
        }
        else
        {
          http_req.method += c;
        }
      }
      else if (state == PossibleStates::URI)
      {
        std::string cmp = " HTTP/1.1\r\n";
        if (http_req_str.compare(i, cmp.length(), cmp) == 0)
        {
          next_state = PossibleStates::HEADER_KEY;
          http_req.headers.push_back({});
          i += cmp.length() - 1;
        }
        else
        {
          http_req.uri += c;
        }
      }
      else if (state == PossibleStates::HEADER_KEY)
      {
        if (c == ":")
        {
          next_state = PossibleStates::HEADER_VAL;
          i += 1;
        }
        else
        {
          http_req.headers.back().key += c;
        }
      }
      else if (state == PossibleStates::HEADER_VAL)
      {
        std::string cmp = "\r\n";
        if (http_req_str.compare(i, cmp.length(), cmp) == 0)
        {
          next_state = PossibleStates::NEXT_HEADER;
          i += cmp.length() - 1;
        }
        else
        {
          http_req.headers.back().val += c;
        }
      }
      else if (state == PossibleStates::NEXT_HEADER)
      {
        std::string cmp = "\r\n";
        if (http_req_str.compare(i, cmp.length(), cmp) == 0)
        {
          next_state = PossibleStates::BODY;
          i += cmp.length() - 1;
        }
        else
        {
          http_req.headers.push_back({});
          i -= 1;
          next_state = PossibleStates::HEADER_KEY;
        }
      }
      else if (state == PossibleStates::BODY)
      {
        http_req.body += c;
      }
      else
      {
        assert(false);
        return HttpRequest{};
      }

      state = next_state;
    }
    return http_req;
  }
  std::map<std::string, std::string> file_ext_to_mime_type{{".txt", "text/plain"}, {".html", "text/html"}, {".css", "text/css"}, {".js", "text/javascript"}, {".png", "image/png"}, {".webp", "image/webp"}, {".php", "text/html"}, {".ico", "image/x-icon"}};
  std::string get_mime_type(const std::string &file_ext)
  {
    return file_ext_to_mime_type[file_ext];
  }
}