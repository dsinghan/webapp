#include "echo_request_handler.h"
#include "gtest/gtest.h"
#include <string>

class requestHandlerTest : public ::testing::Test {
 protected:
  http::server::echo_request_handler han;
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