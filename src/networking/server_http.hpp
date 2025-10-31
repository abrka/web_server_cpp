#pragma once

#include <iostream>
#include <map>
#include <functional>
#include <utility>

#include "net.hpp"
#include "http.hpp"
#include "uri.hpp"
#include "utils.hpp"
#include "php.hpp"

namespace ServerHTTP
{

  using http_req_handler_func_t = std::function<HTTP::HttpResponse(const HTTP::HttpRequest &)>;

  using http_req_handler_key = std::pair<std::string, std::string>; // first item is the uri path, second item the the http method

  struct Server
  {
  private:
    int sockfd = -1;
    std::map<http_req_handler_key, http_req_handler_func_t> http_req_handler_map{};

  public:
    void init(const char *port, int backlog)
    {
      sockfd = Net::socket();
      Net::remove_addr_already_in_use(sockfd);
      Net::bind(sockfd, NULL, port);
      Net::listen(sockfd, backlog);
    }

    void listen()
    {
      while (true)
      {
        ServerError ret = loop();
        if (ret != ServerError::OK)
        {
          std::cout << "SERVER ERROR: " << (int)ret << std::endl;
        }
      }
    }
    void register_http_req_handler(const std::string &uri_path, const std::string &http_method, http_req_handler_func_t handler_func)
    {
      http_req_handler_map[{uri_path, http_method}] = handler_func;
    }

    enum class ServerError
    {
      CANNOT_OPEN_REQ_FILE = 1,
      CANNOT_FIND_MIME_TYPE = 2,
      HTTP_METHOD_DOESNT_MATCH_HANDLER_METHOD = 3,
      NO_VALID_HANDLER_FOR_REQUEST = 4,
      OK = 0
    };

  private:
    ServerError send_local_file(int new_socket, HTTP::HttpRequest http_req)
    {
      std::string response_msg_body{};

      URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);
      std::string local_file_path = get_filepath_from_uri(http_req_uri.path);

      std::string file_ext = get_extension_of_file(local_file_path);
      bool should_file_be_parsed_with_php = file_ext == ".php" || file_ext == ".html";
      if (should_file_be_parsed_with_php)
      {
        response_msg_body = parse_file_with_php(local_file_path);
      }
      else
      {
        int file_opened_successfully = read_str_from_file(local_file_path, response_msg_body);
        if (file_opened_successfully != 0)
        {
          HTTP::HttpResponse response{404, "ERROR", "text/plain", "Put Error Page Here"};
          socket_send_http_response(new_socket, response);
          return ServerError::CANNOT_OPEN_REQ_FILE;
        }
      }

      std::string mime_type = HTTP::get_mime_type(file_ext);
      if (mime_type.empty())
      {
        HTTP::HttpResponse response{404, "ERROR", "text/plain", "Put Mime Type Error Page Here"};
        socket_send_http_response(new_socket, response);
        return ServerError::CANNOT_FIND_MIME_TYPE;
      }

      HTTP::HttpResponse response{200, "OK", mime_type, response_msg_body};
      socket_send_http_response(new_socket, response);

      return ServerError::OK;
    }

    ServerError loop()
    {
      ServerError ret = ServerError::OK;

      sockaddr their_addr;
      int new_socket = Net::accept(sockfd, &their_addr);

      std::string received_msg_str = socket_recv_string(new_socket);
      if (received_msg_str.empty())
      {
        Net::close(new_socket);
        return ret;
      }
      HTTP::HttpRequest http_req = HTTP::parse_http_req_str(received_msg_str);
      URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);

      std::cout << "accepted request: " << http_req_uri.path << "\n";

      if (http_req_handler_map.contains({http_req_uri.path, http_req.method}))
      {
        auto handler = http_req_handler_map[{http_req_uri.path, http_req.method}];
        HTTP::HttpResponse response = handler(http_req);
        socket_send_http_response(new_socket, response);
      }

      else if (http_req.method == "GET")
      {
        ret = send_local_file(new_socket, http_req);
      }
      else
      {
        HTTP::HttpResponse response{404, "ERROR", "text/plain", "Couldn't find valid handler for request"};
        socket_send_http_response(new_socket, response);
        ret = ServerError::NO_VALID_HANDLER_FOR_REQUEST;
      }
      Net::close(new_socket);
      return ret;
    }
  };
}