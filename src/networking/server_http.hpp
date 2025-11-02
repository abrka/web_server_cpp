#pragma once

#include <iostream>
#include <map>
#include <functional>
#include <utility>
#include <filesystem>

#include "net.hpp"
#include "http.hpp"
#include "uri.hpp"
#include "file_utils.hpp"
#include "net_utils.hpp"
#include "string_utils.hpp"
#include "php.hpp"

namespace ServerHTTP
{

  enum class ServerError
  {
    OK = 0,
    HTTP_REQ_PARSE_ERROR,
    SOCKET_RECV_ERROR,
    SOCKET_SEND_ERROR,
    CANNOT_OPEN_REQ_FILE,
    CANNOT_FIND_MIME_TYPE,
    HTTP_METHOD_DOESNT_MATCH_HANDLER_METHOD,
    NO_VALID_HANDLER_FOR_REQUEST,
  };

  using http_req_handler_func_t = std::function<HTTP::HttpResponse(const HTTP::HttpRequest &)>;

  using http_req_handler_key = std::pair<std::string, std::string>; // first item is the uri path, second item the the http method

  struct Server
  {
    std::filesystem::path mount_point{};

  private:
    int sockfd = -1;
    std::map<http_req_handler_key, http_req_handler_func_t> http_req_handler_map{};

  public:
    void init(const char *ip, const char *port, int backlog, int ai_flags = 0)
    {
      sockfd = Net::socket();
      Net::remove_addr_already_in_use(sockfd);
      Net::bind(sockfd, ip, port, ai_flags);
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

  private:
    ServerError send_local_file(int new_socket, HTTP::HttpRequest http_req)
    {
      std::string response_msg_body{};

      URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);
      std::filesystem::path rel_file_path = get_filepath_from_uri(http_req_uri.path);
      std::string local_file_path = mount_point / rel_file_path;

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

      sockaddr their_addr;
      int new_socket = Net::accept(sockfd, &their_addr);

      std::string received_msg_str{};
      auto err_1 = socket_recv_http_string(new_socket, received_msg_str);

      if (err_1 != SocketRecvHttpStringError::OK)
      {
        // there is no point sending a response since the socket's read buffer is not cleared fully. the client will not receive a valid message
        Net::close(new_socket);
        return ServerError::SOCKET_RECV_ERROR;
      }

      // handle 0 bytes in recv
      if (received_msg_str.empty())
      {
        Net::close(new_socket);
        return ServerError::OK;
      }

      HTTP::HttpRequest http_req{};
      auto err_2 = HTTP::parse_http_req_str(received_msg_str, http_req);
      if (err_2 != HTTP::HttpRequestParserError::OK)
      {
        HTTP::HttpResponse response{404, "ERROR", "text/plain", "Error Parsing HTTP Request"};
        socket_send_http_response(new_socket, response);
        Net::close(new_socket);
        return ServerError::HTTP_REQ_PARSE_ERROR;
      }

      URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);

      std::cout << "received message " << http_req_uri.path << "\n";
      if (http_req_handler_map.contains({http_req_uri.path, http_req.method}))
      {
        auto handler = http_req_handler_map[{http_req_uri.path, http_req.method}];
        HTTP::HttpResponse response = handler(http_req);
        socket_send_http_response(new_socket, response);
        Net::close(new_socket);
        return ServerError::OK;
      }

      else if (http_req.method == "GET")
      {
        auto err_3 = send_local_file(new_socket, http_req);
        Net::close(new_socket);
        return err_3;
      }
      else
      {
        HTTP::HttpResponse response{404, "ERROR", "text/plain", "Couldn't find valid handler for request"};
        socket_send_http_response(new_socket, response);
        Net::close(new_socket);
        return ServerError::NO_VALID_HANDLER_FOR_REQUEST;
      }
      assert(false && "SERVER shouldnt reach here");
    }
  };
}