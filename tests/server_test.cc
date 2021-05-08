#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "config_parser.h"
#include "echo_request_handler.h"
#include "server.h"
#include "static_request_handler.h"
#include "error_handler.h"

using ::testing::_;


class ServerTest : public :: testing::Test {
protected:
  NginxConfigParser config_parser;
  NginxConfig config;
};


class MockSession : public session {
    public:
    MockSession(boost::asio::io_service& io_service, std::map<std::string, request_handler*> locations) : session(io_service, locations) {}
    MOCK_METHOD0(start, void());
};

// Ensure that we can instantiate a new server
TEST_F(ServerTest, BasicServer) {
    boost::asio::io_service io_service;
    short port = 8080;
    server * s = nullptr;
    std::map<std::string, request_handler*> locations;
    s = new server(io_service, port, locations);

    EXPECT_TRUE(s != nullptr);

    delete s;
}

// Ensure that we can accept and start a session
TEST_F(ServerTest, ServerHandleAccept) {
    config_parser.Parse("session_config", &config);

    boost::asio::io_service io_service;
    int port = config_parser.extract_port(&config);
    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);
    server * s = new server(io_service, port, locations);

    session * curr_sess = new session(io_service, locations);
    boost::system::error_code error;
    int ret = s->handle_accept(curr_sess,error);

    EXPECT_EQ(ret,0);
}

// TEST(ServerHandleTest, Server) {
//     boost::asio::io_service io_service;
//     MockSession session(io_service);
//     short port = 8080;
//     server * s = nullptr;
//     std::map<std::string, std::string> locations;
//     s = new server(session.io_service, port, locations);
//     echo_request_handler* echo_handler_ = new echo_request_handler();
//     static_request_handler* static_handler_ = new static_request_handler();
//     session * curr_sess = new session(io_service, *echo_handler_, *static_handler_, locations);

//     boost::system::error_code error;
//     int ret = s->handle_accept(curr_sess,error);

//     EXPECT_CALL(session,start()).Times(testing::AtLeast(1));
// }