#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "chat_request_handler.h"

class chatRequestHandlerTest : public ::testing::Test { 
 protected:
  NginxConfig config;
  NginxConfigParser config_parser;
  request_handler *han;
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(chatRequestHandlerTest, handleNormalChatRequest) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/chat")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/chat");

    response = han->handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/html");
    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
}

TEST_F(chatRequestHandlerTest, handleNotFoundStaticRequest) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/chat")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/chat/idk.txt");

    response = han->handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/html");
    EXPECT_TRUE(response["Content-Length"] == "13");
    EXPECT_TRUE(response.body() == "404 Not Found");
    EXPECT_TRUE(response.result() == boost::beast::http::status::not_found);
}