#pragma once

#include <filesystem>
#include <fstream>
#include "net.hpp"
#include "http.hpp"

std::string get_extension_of_file(const std::string file)
{
  return std::filesystem::path(file).extension().string();
}

std::string get_local_path_from_uri_path(const std::string &uri)
{

  std::string path = uri;
  path.erase(0, 1);
  return path;
}

std::string get_filepath_from_uri(const std::string &uri_path)
{
  if (uri_path == "/")
  {
    return "index.html";
  }
  else
  {
    return get_local_path_from_uri_path(uri_path);
  }
}

#define RECEIVED_MSG_SIZE 10000
std::string socket_recv_string(int sockfd)
{
  char received_msg[RECEIVED_MSG_SIZE + 1];
  int bytes_read = Net::recv(sockfd, received_msg, RECEIVED_MSG_SIZE);
  if (bytes_read == 0)
  {
    printf("ERROR: recv_string() bytes read is 0");
    exit(-1);
  }
  received_msg[bytes_read] = '\0';
  return std::string(received_msg);
}

int socket_send_http_response(int sockfd, const HTTP::HttpResponse &response)
{
  std::string http_response_str = HTTP::http_response_to_str(response);
  int bytes_sent = Net::send(sockfd, (void *)http_response_str.c_str(), http_response_str.length());
  if (bytes_sent == (int)http_response_str.length())
  {
    return 0;
  }
  else
  {
    return -1;
  }
}