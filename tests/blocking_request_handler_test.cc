#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "blocking_request_handler.h"

class blockingRequestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  blocking_request_handler han{"Blah", config};
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(blockingRequestHandlerTest, handleNormalRequest) {
    
    response = han.handle_request(request);

    EXPECT_TRUE(response["Content-Length"] == "32");
    EXPECT_TRUE(response.body() == "Successfully put server to sleep");
    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
}

TEST_F(blockingRequestHandlerTest, handleGetName) {
    EXPECT_TRUE(han.get_name() == "Blocking Request Handler");
}