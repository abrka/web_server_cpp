#pragma once

#include <filesystem>
#include <fstream>
#include "net.hpp"
#include "http.hpp"

#include "string_utils.hpp"

// #define RECEIVED_MSG_SIZE 100000
// std::string socket_recv_string(int sockfd)
// {
//   char received_msg[RECEIVED_MSG_SIZE];
//   int bytes_read = Net::recv(sockfd, received_msg, RECEIVED_MSG_SIZE);
//   return std::string(received_msg, bytes_read);
// }

enum class SocketRecvHttpStringError
{
  OK = 0,
  NEG_CONTENT_LENGTH,
  REQUEST_SIZE_PASSES_LIMIT, // request is too big
  MAX_ITERATIONS_PASSED,     // request couldnt be parsed within max limit of iterations
};

SocketRecvHttpStringError socket_recv_http_string(int sockfd, std::string &output, size_t max_req_size = (10 * 1024 * 1024), size_t data_eaten_per_recv = (1024 * 1024), size_t max_iterations = 1000)
{
  std::string recv_msg{};

  for (size_t i = 0; i < max_iterations; i++)
  {
    std::string tmp_str(data_eaten_per_recv, 'e');
    auto bytes_read = Net::recv(sockfd, tmp_str.data(), tmp_str.size());
    if (bytes_read == 0)
    {
      return SocketRecvHttpStringError::OK;
    }

    recv_msg += tmp_str.substr(0, (size_t)bytes_read);

    std::string content_length_str{};
    bool success = str_extract_substr_between_delims(recv_msg, "Content-Length: ", "\r\n", content_length_str);

    int content_length{};
    if (success)
    {
      content_length = std::stoi(content_length_str);
      if (content_length < 0)
        return SocketRecvHttpStringError::NEG_CONTENT_LENGTH;
    }
    else
    {
      content_length = 0;
    }
    if (content_length > max_req_size)
    {
      return SocketRecvHttpStringError::REQUEST_SIZE_PASSES_LIMIT;
    }

    std::string header{};
    std::string body{};
    success = str_split_first(recv_msg, "\r\n\r\n", header, body);
    if (!success)
    {
      continue;
    }
    if ((int)body.size() < content_length)
    {
      continue;
    }
    output = recv_msg;
    return SocketRecvHttpStringError::OK;
  }
  return SocketRecvHttpStringError::MAX_ITERATIONS_PASSED;
}

void socket_send_http_response(int sockfd, const HTTP::HttpResponse &response)
{
  std::string http_response_str = HTTP::http_response_to_str(response);
  auto bytes_sent = Net::send(sockfd, (void *)http_response_str.c_str(), http_response_str.length());
  assert((size_t)bytes_sent == http_response_str.length());
}