#include "server_http.hpp"

HTTP::HttpResponse upload_api_handler(const HTTP::HttpRequest &req)
{
  auto headers_map = req.multi_part_data[0].headers;
  std::string content_disposition = headers_map["Content-Disposition"];
  std::string filename = str_extract_substr_between_delims(content_disposition, "; filename=\"", "\"");
  std::string extension = get_extension_of_file(filename);
  std::ofstream file("downloaded_file" + extension);
  std::string data = req.multi_part_data[0].body;
  file << data;
  return HTTP::HttpResponse{200, "OK", "text/plain", "you called file upload api"};
}
int main()
{
  std::cout << "started server example program\n";
  
  ServerHTTP::Server server{};
  server.register_http_req_handler("/api_call", "GET", [](const HTTP::HttpRequest &req)
                                   { return HTTP::HttpResponse{200, "OK", "text/plain", "blah blah"}; });
  server.register_http_req_handler("/upload_api", "POST", upload_api_handler);
  server.init("3480", 10);
  server.listen();
}