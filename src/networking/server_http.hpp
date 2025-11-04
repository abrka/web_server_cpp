#pragma once

#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <utility>
#include <vector>

#include "file_utils.hpp"
#include "http.hpp"
#include "net.hpp"
#include "net_utils.hpp"
#include "php.hpp"
#include "string_utils.hpp"
#include "uri.hpp"

namespace ServerHTTP {

enum class ServerError {
  OK = 0,
  HTTP_REQ_PARSE_ERROR,
  SOCKET_RECV_ERROR,
  SOCKET_SEND_ERROR,
  CANNOT_OPEN_REQ_FILE,
  NO_VALID_HANDLER_FOR_REQUEST,
};

std::map<ServerError, std::string> server_err_to_string_map{
    {ServerError::OK, "OK"},
    {ServerError::HTTP_REQ_PARSE_ERROR, "error parsing http request"},
    {ServerError::SOCKET_RECV_ERROR, "error receiving data from socket"},
    {ServerError::SOCKET_SEND_ERROR, "error sending data to socket"},
    {ServerError::CANNOT_OPEN_REQ_FILE, "error opening requested file/uri"},
    {ServerError::NO_VALID_HANDLER_FOR_REQUEST,
     "error no handler function found for requested uri"}};

using http_req_handler_func_t =
    std::function<HTTP::HttpResponse(const HTTP::HttpRequest &)>;

struct Server {
  std::filesystem::path mount_point{};
  std::filesystem::path index_file{"index.html"};

private:
  int sockfd = -1;
  struct http_req_handler {
    std::regex uri_path_matcher{};
    std::string http_method{};
    http_req_handler_func_t handler_func{};
  };

  std::vector<http_req_handler> http_req_handler_map{};

public:
  void init(const char *ip, const char *port, int backlog, int ai_flags = 0) {
    sockfd = Net::socket();
    Net::remove_addr_already_in_use(sockfd);
    Net::bind(sockfd, ip, port, ai_flags);
    Net::listen(sockfd, backlog);
  }

  void listen() {
    while (true) {
      ServerError ret = loop();
      if (ret != ServerError::OK) {
        std::cout << "SERVER ERROR: " << server_err_to_string_map[ret]
                  << std::endl;
      }
    }
  }
  void register_http_req_handler(const std::regex &uri_path_matcher,
                                 const std::string &http_method,
                                 http_req_handler_func_t handler_func) {
    http_req_handler_map.push_back(
        {uri_path_matcher, http_method, handler_func});
  }

private:
  bool load_local_file_into_http_response(const HTTP::HttpRequest &http_req,
                                          HTTP::HttpResponse &http_res) {
    std::string response_msg_body{};

    URI::URI http_req_uri = URI::parse_uri_from_string(http_req.uri);
    std::filesystem::path rel_file_path =
        (http_req_uri.path == "/")
            ? index_file
            : get_filepath_from_uri_path(http_req_uri.path);

    std::string local_file_path = mount_point / rel_file_path;
    
    std::string file_ext = get_extension_of_file(local_file_path);
    bool should_file_be_parsed_with_php = file_ext == ".php";
    if (should_file_be_parsed_with_php) {
      response_msg_body = parse_file_with_php(local_file_path);
    } else {
      int file_opened_successfully =
          read_str_from_file(local_file_path, response_msg_body);
      if (file_opened_successfully != 0) {
        HTTP::HttpResponse response{404,
                                    "ERROR",
                                    {{"Content-Type", "text/plain"}},
                                    "Put Error Page Here"};
        http_res = response;
        return false;
      }
    }

    std::string mime_type = HTTP::get_mime_type(file_ext);
    if (mime_type.empty()) {
      HTTP::HttpResponse response{404,
                                  "ERROR",
                                  {{"Content-Type", "text/plain"}},
                                  "Put Mime Type Error Page Here"};
      http_res = response;
      return false;
    }
    HTTP::HttpResponse response{
        200, "OK", {{"Content-Type", mime_type}}, response_msg_body};

    http_res = response;
    return true;
  }

  /**
   * @warning updates http request with uri capture params
   */
  bool find_http_req_handler_func(HTTP::HttpRequest &req,
                                  http_req_handler_func_t &handler_func) {
    URI::URI req_uri = URI::parse_uri_from_string(req.uri);

    for (const auto &handler : http_req_handler_map) {
      if (handler.http_method != req.method) {
        continue;
      }
      std::smatch regex_match{};
      bool found_match =
          std::regex_match(req_uri.path, regex_match, handler.uri_path_matcher);
      if (found_match) {
        handler_func = handler.handler_func;
        for (size_t i = 0; i < regex_match.size(); ++i) {
          req.custom_params.push_back(regex_match[i].str());
        }
        return true;
      }
    }
    return false;
  }

  ServerError loop() {

    sockaddr their_addr;
    int new_socket = Net::accept(sockfd, &their_addr);

    std::string received_msg_str{};
    auto err_1 = socket_recv_http_string(new_socket, received_msg_str);

    if (err_1 != SocketRecvHttpStringError::OK) {
      // there is no point sending a response since the socket's read buffer is
      // not cleared fully. the client will not receive a valid message
      Net::close(new_socket);
      return ServerError::SOCKET_RECV_ERROR;
    }

    // handle 0 bytes in recv
    if (received_msg_str.empty()) {
      Net::close(new_socket);
      return ServerError::OK;
    }

    HTTP::HttpRequest http_req{};
    auto err_2 = HTTP::parse_http_req_str(received_msg_str, http_req);
    if (err_2 != HTTP::HttpRequestParserError::OK) {
      HTTP::HttpResponse response{404,
                                  "ERROR",
                                  {{"Content-Type", "text/plain"}},
                                  "Error Parsing HTTP Request"};
      socket_send_http_response(new_socket, response);
      Net::close(new_socket);
      return ServerError::HTTP_REQ_PARSE_ERROR;
    }

    std::cout << "SERVER: received message from uri: " << http_req.uri << "\n";

    http_req_handler_func_t handler{};
    bool found_handler = find_http_req_handler_func(http_req, handler);
    if (found_handler) {
      HTTP::HttpResponse response = handler(http_req);
      socket_send_http_response(new_socket, response);
      Net::close(new_socket);
      return ServerError::OK;
    } else if (http_req.method == "GET") {
      HTTP::HttpResponse response{};
      bool success = load_local_file_into_http_response(
          http_req, response); // no need to check for error and send a response
                               // yourself, the function will send an error
                               // ersponse if it runs itno an error
      socket_send_http_response(new_socket, response);
      Net::close(new_socket);
      if (!success) {
        return ServerError::CANNOT_OPEN_REQ_FILE;
      }
      return ServerError::OK;
    } else {
      HTTP::HttpResponse response{404,
                                  "ERROR",
                                  {{"Content-Type", "text/plain"}},
                                  "Couldn't find valid handler for request"};
      socket_send_http_response(new_socket, response);
      Net::close(new_socket);
      return ServerError::NO_VALID_HANDLER_FOR_REQUEST;
    }
    assert(false && "SERVER shouldnt reach here");
  }
};
} // namespace ServerHTTP