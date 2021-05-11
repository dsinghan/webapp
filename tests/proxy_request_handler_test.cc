#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "proxy_request_handler.h"

class proxyRequestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  NginxConfigParser config_parser;
  request_handler *han;
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(proxyRequestHandlerTest, handleNormalProxyRequest) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/ucla")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/ucla");

   response = han->handle_request(request);

    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_EQ(response["Content-Length"], "24");
    EXPECT_EQ(response.body(), "Place Holder for Request");
    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
}
