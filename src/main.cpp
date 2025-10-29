#include "server_http.hpp"

int main(){
  ServerHTTP::server_http server{};
  server.init("3490", 10);
  server.listen();
}