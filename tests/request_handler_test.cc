#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "echo_request_handler.h"

class requestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  echo_request_handler han{"Blah", config};
  std::string output;
  bool success;
};

TEST_F(requestHandlerTest, urlDecodeTestGood) {
    success = han.url_decode("https://localhost/echo/hello.txt", output);

    EXPECT_TRUE(success);

}

TEST_F(requestHandlerTest, urlDecodeTestBad) {
    success = han.url_decode("https://localhost/echo/hello.txt+hello\%goodbye", output);

    EXPECT_FALSE(success);

}