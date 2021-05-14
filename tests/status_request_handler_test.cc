// TODO: Fill this in
#include <string>

#include "gtest/gtest.h"

#include "config_parser.h"
#include "status_request_handler.h"

class statusRequestHandlerTest : public ::testing::Test {
 protected:
  NginxConfig config;
  status_request_handler han{"Blah", config};
  boost::beast::http::request<boost::beast::http::string_body> request;
  boost::beast::http::response<boost::beast::http::string_body> response;
};

TEST_F(statusRequestHandlerTest, bad_report) {
    request.method(boost::beast::http::verb::get);
    request.target("/status");

    response = han.handle_request(request);

    // TODO: Expects
    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_EQ(response.body(), "No Request Results Available\n");
}

TEST_F(statusRequestHandlerTest, generate_simple_report) {
    std::map<std::pair<std::string, int>, int> request_results;
    han.set_request_results(&request_results);
    request.method(boost::beast::http::verb::get);
    request.target("/status");

    response = han.handle_request(request);

    // TODO: Expects
    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_EQ(response.body(), "Report:\n\nRequests Received:\n\nRequest Handlers:\n");
}

TEST_F(statusRequestHandlerTest, generate_complicated_report) {
    std::map<std::pair<std::string, int>, int> request_results;
    request_results[std::make_pair("/echo", 200)] = 5;
    request_results[std::make_pair("/", 404)] = 3;
    han.set_request_results(&request_results);

    std::map<std::string, std::vector<std::string> > handlers_url_map;
    std::vector<std::string> urls;
    urls.push_back("/echo");
    handlers_url_map["EchoHandler"] = urls;
    han.set_handlers_url_map(handlers_url_map);

    request.method(boost::beast::http::verb::get);
    request.target("/status");

    response = han.handle_request(request);

    // TODO: Expects
    EXPECT_TRUE(response["Content-Type"] == "text/plain");
    EXPECT_EQ(response.body(), "Report:\n\nRequests Received:\n/, 404: 3\n/echo, 200: 5\n\nRequest Handlers:\nEchoHandler\n\t/echo\n");
}