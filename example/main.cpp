#include "server_http.hpp"
#include <unistd.h>

HTTP::HttpResponse upload_api_handler(const HTTP::HttpRequest &req)
{
  static int count = 0;

  auto headers_map = req.multi_part_data[0].headers;
  std::string content_disposition = headers_map["Content-Disposition"];
  std::string filename = str_extract_substr_between_delims(content_disposition, "; filename=\"", "\"");
  std::string extension = get_extension_of_file(filename);
  std::ofstream file("downloaded_file_" + std::to_string(count) + extension);
  std::string data = req.multi_part_data[0].body;
  file << data;

  count++;
  return HTTP::HttpResponse{200, "OK", "text/plain", "you called file upload api"};
}

int main(int argc, char *argv[])
{

  std::cout << "started server example program\n";

  std::string port = "3490";
  std::string mount_point = "assets";


  int opt = -1;

  while ((opt = getopt(argc, argv, "p:m:")) != -1)
  {
    if (opt == 'p')
    {
      port = std::string(optarg);
    }
    else if (opt == 'm')
    {
      mount_point = std::string(optarg);
    }
    else if (opt == '?')
    {
      printf("unrecognized option");
      exit(-1);
    }
  }

  ServerHTTP::Server server{};
  server.mount_point = mount_point;

  server.register_http_req_handler("/api_call", "GET", [](const HTTP::HttpRequest &req)
                                   { return HTTP::HttpResponse{200, "OK", "text/plain", "blah blah from c++ backend"}; });
  server.register_http_req_handler("/upload_api", "POST", upload_api_handler);

  server.init(port.c_str(), 10);
  server.listen();
}