#include "gtest/gtest.h"
#include "session.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "config_parser.h"

#include <string>

class sessionTest : public ::testing::Test {
 protected:
    boost::asio::io_service io_service;
    NginxConfigParser parser;
    NginxConfig config;
    bool value = parser.Parse("sample_configs/sessionConfig", &config);
    std::map<std::string, request_handler*> locations = parser.get_locations(&config);
    std::map<std::pair<std::string, int>, int> * request_results;
    session * mySession = new session(io_service, locations, request_results);
    bool success;
};

//Test a good handle_read input
TEST_F(sessionTest, testGetLocation) {

    EXPECT_EQ(locations.size(),4);

}

TEST_F(sessionTest, testPathDetermination) {
    boost::beast::http::request<boost::beast::http::string_body> request;
    std::string ret;

    request.target("/");
    ret = mySession->determine_path(request);
    EXPECT_EQ(ret, "/");

    request.target("");
    mySession->determine_path(request);
    EXPECT_EQ(ret, "/");

    request.target("/../blah");
    mySession->determine_path(request);
    EXPECT_EQ(ret, "/");

    request.target("abcde");
    mySession->determine_path(request);
    EXPECT_EQ(ret, "/");

    request.target("/static1/index.html");
    ret = mySession->determine_path(request);
    EXPECT_EQ(ret, "/static1");

    request.target("/static1/");
    ret = mySession->determine_path(request);
    EXPECT_EQ(ret, "/static1");

    request.target("/static1/dir_name/dir_name");
    ret = mySession->determine_path(request);
    EXPECT_EQ(ret, "/static1/dir_name");
}

//Tests to make sure errors are handled properly in handle_read
TEST_F(sessionTest, SessionTest) {

    boost::system::error_code error = boost::system::errc::make_error_code(boost::system::errc::not_supported);

    int ret = mySession->handle_read(error, 100);

    EXPECT_EQ(ret, 1);
}

//Tests to make sure errors are handled properly in handle_write
TEST_F(sessionTest, testSession) {

    boost::system::error_code error;
    error.assign(1, boost::system::system_category());

    int ret = mySession->handle_write(error);

    EXPECT_EQ(ret, 1);
}

//Tests to make sure proper socket is returned in socket function
TEST_F(sessionTest, testSocket) {

    boost::asio::ip::tcp::socket * sock = &mySession->socket();
    int ret = (&mySession->socket_ == sock);

    EXPECT_TRUE(ret);
}

//Tests to make sure proper socket is returned in socket function
TEST_F(sessionTest, testPathExtensionRemoval) {
    EXPECT_EQ(mySession->remove_path_extension("/hello/"), "/hello");
    EXPECT_EQ(mySession->remove_path_extension("/hello/hello"), "/hello");
    EXPECT_EQ(mySession->remove_path_extension("/hello"), "/");
    EXPECT_EQ(mySession->remove_path_extension("hello"), "/");
}

