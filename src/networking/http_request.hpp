#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <assert.h>
#include <sstream>
#include <map>

#include "http_parser_string_stream.hpp"
#include "string_utils.hpp"

namespace HTTP
{

  struct HttpMultiPartDataPacket
  {
    std::map<std::string, std::string> headers{};
    std::string body{};
  };

  struct HttpRequest
  {
    std::string method{};
    std::string uri{};
    std::map<std::string, std::string> headers{};
    std::string body{};
    std::string version{};
    std::vector<HttpMultiPartDataPacket> multi_part_data{};
  };

  bool parse_http_method(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string method = stream.seek_until(" ");
    if (method.empty())
    {
      return false;
    }
    http_req.method = method;
    return true;
  }
  bool parse_http_uri(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string uri = stream.seek_until(" ");
    if (uri.empty())
    {
      return false;
    }
    http_req.uri = uri;
    return true;
  }
  bool parse_http_version(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string version = stream.seek_until("\r\n");
    if (version.empty())
    {
      return false;
    }
    http_req.version = version;
    return true;
  }
  bool parse_http_req_line(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    return parse_http_method(stream, http_req) && parse_http_uri(stream, http_req) && parse_http_version(stream, http_req);
  }
  bool parse_http_header(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string key = stream.seek_until(": ");
    std::string val = stream.seek_until("\r\n");
    if (key.empty() || val.empty())
    {
      return false;
    }
    http_req.headers[key] = val;
    return true;
  }
  bool parse_http_headers(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    while (stream.match("\r\n") == false)
    {
      bool success = parse_http_header(stream, http_req);
      if (!success)
      {
        return false;
      }
    }
    stream.pos += strlen("\r\n");
    return true;
  }
  bool parse_http_body(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    http_req.body = stream.get();
    return true;
  }
  bool parse_http_multi_part_header(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string key = stream.seek_until(": ");
    std::string val = stream.seek_until("\r\n");
    if (key.empty() || val.empty())
    {
      return false;
    }
    (http_req.multi_part_data.back()).headers[key] = val;
    return true;
  }
  bool parse_http_multi_part_headers(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    while (stream.match("\r\n") == false)
    {
      bool success = parse_http_multi_part_header(stream, http_req);
      if (!success)
      {
        return false;
      }
    }
    stream.pos += strlen("\r\n");
    return true;
  }
  bool parse_http_multi_part_body(HttpParserStringStream &stream, HttpRequest &http_req)
  {
    std::string& body = http_req.multi_part_data.back().body;
    body = stream.get();
    body.pop_back();
    body.pop_back();
    return true;
  }
  HttpRequest parse_http_req_str(const std::string &http_req_str)
  {
    HttpParserStringStream stream{0, http_req_str};
    HttpRequest output_req{};
    bool success = true;

    success = parse_http_req_line(stream, output_req);
    success = success && parse_http_headers(stream, output_req);
    success = success && parse_http_body(stream, output_req);

    std::string multi_part_form_data = "multipart/form-data";
    std::string content_type_header_val = output_req.headers["Content-Type"];
    bool is_req_multi_part = content_type_header_val.compare(0, multi_part_form_data.size(), multi_part_form_data) == 0;
    if (is_req_multi_part)
    {
      std::string output_req_body_copy = output_req.body;
      std::string boundary_str = str_split(content_type_header_val, "=")[1];
      // remove the last boundary string(special case)
      std::string remove_at = "--" + boundary_str + "--\r\n";
      str_remove_substr(output_req_body_copy, remove_at);

      // split into multi part datas
      std::string split_at = "--" + boundary_str + "\r\n";
      std::vector<std::string> multi_part_datas = str_split(output_req_body_copy, split_at);
      for (const auto &multi_part_data : multi_part_datas)
      {
        if (multi_part_data.empty())
        {
          continue;
        }
        output_req.multi_part_data.push_back({});
        HttpParserStringStream multi_part_stream{0, multi_part_data};
        success = success && parse_http_multi_part_headers(multi_part_stream, output_req);
        success = success && parse_http_multi_part_body(multi_part_stream, output_req);
      }
    }

    assert(success);
    return output_req;
  }

}