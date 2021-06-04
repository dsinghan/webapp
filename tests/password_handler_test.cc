#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "password_handler.h"

class passwordHandlerTest : public ::testing::Test { 
 protected:
  NginxConfig config;
  NginxConfigParser config_parser;
  request_handler *han;
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(passwordHandlerTest, correctPasswordInput) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/check_password")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/check_password");
    request.set(boost::beast::http::field::security_scheme, "goBruins");

    response = han->handle_request(request);

    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
    EXPECT_TRUE(response.body() != "False");
}

TEST_F(passwordHandlerTest, incorrectPasswordInput) {
    config_parser.Parse("sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/check_password")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/check_password");
    request.set(boost::beast::http::field::security_scheme, "incorrectPassword");

    response = han->handle_request(request);

    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
    EXPECT_TRUE(response.body() == "False");
}