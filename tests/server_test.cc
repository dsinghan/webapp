#include "server.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"


using ::testing::_;


class MockSession : public session {
    public:
    MockSession(boost::asio::io_service& io_service, std::map<std::string, http::server::request_handler*> locations) : session(io_service, locations) {}
    MOCK_METHOD0(start, void());
};

// Test basic functionality of server
TEST(ServerTest, Server) {
    boost::asio::io_service io_service;
    short port = 8080;
    server * s = nullptr;
    std::map<std::string, http::server::request_handler*> locations;
    s = new server(io_service, port, locations);

    EXPECT_TRUE(s != nullptr);

    delete s;
}

//test the handle_test function
TEST(ServerHandleTest, Server) {
    boost::asio::io_service io_service;
    short port = 8080;
    server * s = nullptr;
    std::string doc_root = ".";
    std::map<std::string, http::server::request_handler*> locations;
    s = new server(io_service, port, locations);
    http::server::echo_request_handler* echo_handler_ = new http::server::echo_request_handler();
    http::server::static_request_handler* static_handler_ = new http::server::static_request_handler(doc_root);
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
//     http::server::echo_request_handler* echo_handler_ = new http::server::echo_request_handler();
//     http::server::static_request_handler* static_handler_ = new http::server::static_request_handler();
//     session * curr_sess = new session(io_service, *echo_handler_, *static_handler_, locations);

//     boost::system::error_code error;
//     int ret = s->handle_accept(curr_sess,error);

//     EXPECT_CALL(session,start()).Times(testing::AtLeast(1));
// }