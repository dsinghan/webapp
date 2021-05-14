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

TEST_F(proxyRequestHandlerTest, handleRedirProxyRequest) {
    config_parser.Parse("../tests/sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/washington")->second;

    request.method(boost::beast::http::verb::get);
    request.target("/washington");

    response = han->handle_request(request);
    
    EXPECT_TRUE(response["Content-Type"] == "text/html");
    EXPECT_TRUE(response.result() == boost::beast::http::status::ok);
}

TEST_F(proxyRequestHandlerTest, formURI) {
    config_parser.Parse("../tests/sample_configs/example_config", &config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    han = locations.find("/washington")->second;
    proxy_request_handler* phan = (proxy_request_handler*) han;

    request.target("/washington/index/cat.html");
    std::string URI = phan->form_URI(request);
    EXPECT_EQ(URI, "/index/cat.html");
}
