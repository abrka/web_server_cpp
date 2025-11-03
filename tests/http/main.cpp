#include "http.hpp"
#include <gtest/gtest.h>

TEST(HttpRequestParserTest, ParsesMethod) {
  HTTP::HttpParserStringStream stream{0, "GET / HTTP/1.1\r\n"};
  HTTP::HttpRequest req{};
  ASSERT_TRUE(HTTP::parse_http_method(stream, req));
  ASSERT_EQ(req.method, "GET");

  HTTP::HttpParserStringStream stream2{
      0, "POST /abrka/hello/www.com HTTP/1.1\r\n"};
  HTTP::HttpRequest req2{};
  ASSERT_TRUE(HTTP::parse_http_method(stream2, req2));
  ASSERT_EQ(req2.method, "POST");
}

TEST(HttpRequestParserTest, ParsesURI) {
  HTTP::HttpParserStringStream stream{0, "GET / HTTP/1.1\r\n"};
  HTTP::HttpRequest req{};
  ASSERT_TRUE(HTTP::parse_http_method(stream, req));
  ASSERT_TRUE(HTTP::parse_http_uri(stream, req));
  ASSERT_EQ(req.uri, "/");

  HTTP::HttpParserStringStream stream2{
      0, "POST /abrka/hello/www.com HTTP/1.1\r\n"};
  HTTP::HttpRequest req2{};
  ASSERT_TRUE(HTTP::parse_http_method(stream2, req2));
  ASSERT_TRUE(HTTP::parse_http_uri(stream2, req2));
  ASSERT_EQ(req2.uri, "/abrka/hello/www.com");
}

TEST(HttpRequestParserTest, ParsesVersion) {
  HTTP::HttpParserStringStream stream{0, "GET / HTTP/1.1\r\n"};
  HTTP::HttpRequest req{};
  ASSERT_TRUE(HTTP::parse_http_method(stream, req));
  ASSERT_TRUE(HTTP::parse_http_uri(stream, req));
  ASSERT_TRUE(HTTP::parse_http_version(stream, req));
  ASSERT_EQ(req.version, "HTTP/1.1");

  HTTP::HttpParserStringStream stream2{0,
                                       "POST /abrka/hello/www.com HTTP/2\r\n"};
  HTTP::HttpRequest req2{};
  ASSERT_TRUE(HTTP::parse_http_method(stream2, req2));
  ASSERT_TRUE(HTTP::parse_http_uri(stream2, req2));
  ASSERT_TRUE(HTTP::parse_http_version(stream2, req2));
  ASSERT_EQ(req.version, "HTTP/1.1");
}

TEST(HttpRequestParserTest, ParsesReqLine) {
  HTTP::HttpParserStringStream stream{0, "GET / HTTP/1.1\r\n"};
  HTTP::HttpRequest req{};
  ASSERT_TRUE(HTTP::parse_http_req_line(stream, req));
  ASSERT_EQ(req.method, "GET");
  ASSERT_EQ(req.uri, "/");
  ASSERT_EQ(req.version, "HTTP/1.1");

  HTTP::HttpParserStringStream stream2{0,
                                       "POST /GETabrka/hello/GET HTTP/2\r\n"};
  HTTP::HttpRequest req2{};
  ASSERT_TRUE(HTTP::parse_http_req_line(stream2, req2));
  ASSERT_EQ(req2.method, "POST");
  ASSERT_EQ(req2.uri, "/GETabrka/hello/GET");
  ASSERT_EQ(req2.version, "HTTP/2");
}

TEST(HttpRequestParserTest, ParsesHeaders) {
  HTTP::HttpParserStringStream stream{
      0, "key1: val1\r\nkey2: val2\r\nkey3: val3\r\n\r\n"};
  HTTP::HttpRequest req{};
  ASSERT_TRUE(HTTP::parse_http_headers(stream, req));
  EXPECT_EQ(req.headers["key1"], "val1");
  EXPECT_EQ(req.headers["key2"], "val2");
  EXPECT_EQ(req.headers["key3"], "val3");
}