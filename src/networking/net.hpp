#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

namespace Net
{

  int socket(int domain = AF_INET, int type = SOCK_STREAM)
  {
    int my_socket = ::socket(domain, type, 0);
    if (my_socket == -1)
    {
      perror("ERROR: cant create socket");
      exit(-1);
    }
    return my_socket;
  }
  void remove_addr_already_in_use(int sockfd)
  {
    int yes = 1;
    int ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    if (ret != 0)
    {
      perror("ERROR: cant run setsockopt()");
      exit(-1);
    }
  }

  addrinfo *get_addr(const char *ip, const char *port, int ai_flags = 0)
  {
    addrinfo hints{};
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = ai_flags;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *my_addrinfo;
    int ret = getaddrinfo(ip, port, &hints, &my_addrinfo);
    if (ret != 0)
    {
      printf("ERROR: cant run getaddrinfo(): %s \n", gai_strerror(ret));
      exit(-1);
    }
    for (addrinfo *p = my_addrinfo; p != NULL; p = p->ai_next)
    {
      if (p != NULL)
      {
        return p;
      }
    }
    printf("ERROR: no addrinfo found \n");
    exit(-1);
  }

  void bind(int sockfd, const char *ip, const char *port, int ai_flags = 0)
  {
    addrinfo *my_addrinfo = get_addr(ip, port, ai_flags);
    int ret = ::bind(sockfd, my_addrinfo->ai_addr, my_addrinfo->ai_addrlen);
    if (ret != 0)
    {
      perror("ERROR: cant bind socket");
      exit(-1);
    }
    freeaddrinfo(my_addrinfo);
  }

  void connect(int sockfd, const char *ip, const char *port)
  {
    addrinfo *my_addrinfo = get_addr(ip, port);
    int ret = ::connect(sockfd, my_addrinfo->ai_addr, my_addrinfo->ai_addrlen);
    if (ret == -1)
    {
      perror("ERROR: can't connect socket");
      exit(-1);
    }
    freeaddrinfo(my_addrinfo);
  }
  void listen(int sockfd, int backlog)
  {
    int ret = ::listen(sockfd, backlog);
    if (ret != 0)
    {
      perror("ERROR: cant listen from socket");
      exit(-1);
    }
  }
  int accept(int sockfd, sockaddr *connected_addr)
  {
    socklen_t connected_addr_len = sizeof(sockaddr_storage);
    int new_socket = ::accept(sockfd, connected_addr, &connected_addr_len);
    if (new_socket == -1)
    {
      perror("ERROR: cant accept socket");
      exit(-1);
    }
    return new_socket;
  }
  auto send(int sockfd, void *data, size_t data_size)
  {
    auto bytes_sent = ::send(sockfd, data, data_size, 0);
    if (bytes_sent == -1)
    {
      perror("ERROR: cant send");
      exit(-1);
    }
    return bytes_sent;
  }
  auto recv(int sockfd, void *buffer, size_t buffer_size)
  {
    auto bytes_recv = ::recv(sockfd, buffer, buffer_size, 0);
    if (bytes_recv == -1)
    {
      perror("ERROR: cant recv");
      exit(-1);
    }
    if (bytes_recv == 0)
    {
      printf("HINT: connection closed in recv \n");
    }
    return bytes_recv;
  }
  void close(int sockfd)
  {
    ::close(sockfd);
  }
}
