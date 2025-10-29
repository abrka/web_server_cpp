#pragma once

#include <iostream>
#include <map>
#include <functional>

#include "net.hpp"
#include "http.hpp"
#include "uri.hpp"
#include "utils.hpp"
#include "php.hpp"

namespace ServerHTTP
{

  using http_req_handler_func_t = std::function<HTTP::HttpResponse(const HTTP::HttpRequest&)>;

  struct http_req_handler_t
  {
    std::string http_method{};
    http_req_handler_func_t func{};
  };

  struct server_http
  {

private:
    int sockfd = -1;
    std::map<std::string, http_req_handler_t> http_req_handler_map{};

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
        loop();
      }
    }
    void register_http_req_handler(const std::string& uri_path, const std::string& http_method, http_req_handler_func_t handler_func){
      http_req_handler_map[uri_path] = {http_method, handler_func};
    }

private:
    int send_local_file(int new_socket, HTTP::HttpRequest http_req)
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
          return -1;
        }
      }

      std::string mime_type = HTTP::get_mime_type(file_ext);
      if (mime_type.empty())
      {
        HTTP::HttpResponse response{404, "ERROR", "text/plain", "Put Mime Type Error Page Here"};
        socket_send_http_response(new_socket, response);
        return -1;
      }

      HTTP::HttpResponse response{200, "OK", mime_type, response_msg_body};
      socket_send_http_response(new_socket, response);

      return 0;
    }

    int loop()
    {
      int ret = 0;

      sockaddr their_addr;
      int new_socket = Net::accept(sockfd, &their_addr);

      std::string received_msg_str = socket_recv_string(new_socket);
      HTTP::HttpRequest http_req = HTTP::parse_http_req_str(received_msg_str);
      URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);

      if (http_req_handler_map.contains(http_req_uri.path))
      {
        auto handler = http_req_handler_map[http_req_uri.path];
        if (handler.http_method != http_req.method)
        {
          HTTP::HttpResponse response{404 , "ERROR", "text/plain", "HTTP Method Doesn't match handler method type error"};
          socket_send_http_response(new_socket, response);
          ret = -1;
        }
        HTTP::HttpResponse response = handler.func(http_req);
        socket_send_http_response(new_socket, response);
      }

      else
      {
        ret = send_local_file(new_socket, http_req);
      }
      Net::close(new_socket);
      return ret;
    }
  };
}