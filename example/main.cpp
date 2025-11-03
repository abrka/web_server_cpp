#include <unistd.h>

#include <string>
#include <vector>

#include "server_http.hpp"

static std::map<std::string, std::string> filename_to_downloaded_filename_map{};
static int download_count = 0;

HTTP::HttpResponse post_file_api_handler(const HTTP::HttpRequest &req) {
  auto headers_map = req.multi_part_data[0].headers;
  std::string content_disposition = headers_map["Content-Disposition"];
  std::string filename = str_extract_substr_between_delims_unsafe(
      content_disposition, "; filename=\"", "\"");
  std::string extension = get_extension_of_file(filename);
  std::string download_filename =
      "downloaded_file_" + std::to_string(download_count) + extension;

  filename_to_downloaded_filename_map[filename] = download_filename;
  std::ofstream file(download_filename);
  std::string data = req.multi_part_data[0].body;
  file << data;
  download_count++;
  return HTTP::HttpResponse{
      200, "OK", {{"Content-Type", "text/plain"}}, "you called POST files api"};
}

HTTP::HttpResponse get_files_api_handler(const HTTP::HttpRequest &req) {
  std::stringstream output_json_stream{};
  output_json_stream << "{";
  output_json_stream << "\"filenames\": " << "[";

  size_t i = 0;
  for (const auto &[filename, downloaded_filename] :
       filename_to_downloaded_filename_map) {
    if (i != 0) {
      output_json_stream << ",";
    }

    output_json_stream << "{";

    output_json_stream << " \"filename\": ";
    output_json_stream << "\"" << filename << "\"";
    output_json_stream << ",";
    output_json_stream << "\"downloaded_filename\": ";
    output_json_stream << "\"" << downloaded_filename << "\"";

    output_json_stream << "}";

    i++;
  }

  output_json_stream << "]";
  output_json_stream << "}";

  std::string output_json_str = output_json_stream.str();
  return HTTP::HttpResponse{
      200, "OK", {{"Content-Type", "text/json"}}, output_json_str};
}

int main(int argc, char *argv[]) {
  std::cout << "started server example program\n";

  std::string ip = "0.0.0.0";
  std::string port = "3000";
  std::string mount_point = "assets";

  int opt = -1;

  while ((opt = getopt(argc, argv, "p:m:i:")) != -1) {
    if (opt == 'p') {
      port = std::string(optarg);
    } else if (opt == 'm') {
      mount_point = std::string(optarg);
    } else if (opt == 'i') {
      ip = std::string(optarg);
    } else if (opt == ':') {
      printf("option argument required\n");
      exit(-1);
    } else if (opt == '?') {
      printf("unrecognized option\n");
      exit(-1);
    }
  }

  ServerHTTP::Server server{};
  server.mount_point = mount_point;

  server.register_http_req_handler(std::regex("/files"), "GET",
                                   get_files_api_handler);

  server.register_http_req_handler(std::regex("/files"), "POST",
                                   post_file_api_handler);

  server.register_http_req_handler(
      std::regex("/api_call"), "GET", [](const HTTP::HttpRequest &) {
        return HTTP::HttpResponse{
            200, "OK", {{"Content-Type", "text/plain"}}, "{}"};
      });

  server.register_http_req_handler(
      std::regex(R"(/api_regex_test/(\w*))"), "GET",
      [](const HTTP::HttpRequest &req) {
        std::string match = req.custom_params[1];
        return HTTP::HttpResponse{200,
                                  "OK",
                                  {{"Content-Type", "text/plain"}},
                                  "you called api regex test with " + match};
      });

  server.init(ip.c_str(), port.c_str(), 10);
  server.listen();
}