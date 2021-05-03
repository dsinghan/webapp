#include "gtest/gtest.h"
#include "request_parser.h"
#include "request.h"


class RequestParserTest : public ::testing::Test {
 protected:
  http::server::request_parser parser;

  http::server::request request_;
  int max_length = 1024;
  char data_[1024];
  http::server::request_parser::result_type result;

  bool success;
};

TEST_F(RequestParserTest, ResetParser) {


  parser.parse(request_, data_, data_ + max_length);
  parser.reset();
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + max_length);
  success = result == http::server::request_parser::bad;

  EXPECT_TRUE(success);
}

TEST_F(RequestParserTest, ParseBad) {

  std::string s;

  s = "GET /echo HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo XTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HXTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HTXP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HTTX/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HTTP/X.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HTTP/1.X\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);

  s = "GET /echo HTTP/1.1\r\rHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept:*/*\r\n\r\n";
  for (int i = 0; i < s.length(); i++) data_[i] = s[i];
  std::tie(result, std::ignore) = parser.parse(request_, data_, data_ + s.length());
  success = result == http::server::request_parser::bad;
  EXPECT_TRUE(success);
}
