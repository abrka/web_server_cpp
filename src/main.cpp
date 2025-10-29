#include "server_http.hpp"

int main()
{
  ServerHTTP::server_http server{};
  server.register_http_req_handler("/api_call", "GET", [](const HTTP::HttpRequest &)
                                   { return HTTP::HttpResponse{200, "OK", "text/plain", "blah blah"}; });
  server.init("3490", 10);
  server.listen();
}