#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "echo_request_handler.h"

class echoRequestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  echo_request_handler han{"Blah", config};
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(echoRequestHandlerTest, handleEmptyRequest) {
    
    response = han.handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_TRUE(response["Content-Length"] == "12");
    EXPECT_TRUE(response.body() == "  HTTP/1.1\r\n");

}

TEST_F(echoRequestHandlerTest, handleSimpleRequest) {
    
    request.method(boost::beast::http::verb::get);
    request.set("User-Agent", "https://www.cs130.org/");
    
    response = han.handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_TRUE(response["Content-Length"] == "51");
    EXPECT_TRUE(response.body() == "GET  HTTP/1.1\r\nUser-Agent: https://www.cs130.org/\r\n");

}

TEST_F(echoRequestHandlerTest, handleGetName) {
    EXPECT_TRUE(han.get_name() == "Echo Request Handler");
}