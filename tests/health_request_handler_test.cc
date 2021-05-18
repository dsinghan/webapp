#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "health_request_handler.h"

class healthRequestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  NginxConfigParser config_parser;
  request_handler *han;
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(healthRequestHandlerTest, checkHealth) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/health")->second;

    response = han->handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_TRUE(response["Content-Length"] == "2");
    EXPECT_TRUE(response.body() == "OK");
    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
}
