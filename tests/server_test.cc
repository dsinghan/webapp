#include "server.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"


using ::testing::_;

class MockSession : public session {
    public:
    MockSession(boost::asio::io_service& io_service, http::server::echo_request_handler& echo_handler, http::server::static_request_handler& static_handler) : session(io_service, echo_handler, static_handler) {}
    MOCK_METHOD0(start, void());
};


//Test basic functionality of server
// TEST(ServerTest, Server) {
//     boost::asio::io_service io_service;
//     short port = 8080;
//     server * s = nullptr;
//     s = new server(io_service, port, ".");

//     EXPECT_TRUE(s != nullptr);

//     delete s;
// }

// //test the handle_test function
// TEST(ServerHandleTest, Server) {
//     boost::asio::io_service io_service;
//     short port = 8080;
//     server * s = nullptr;
//     std::string doc_root = ".";
//     s = new server(io_service, port, doc_root);
//     http::server::request_handler r = http::server::request_handler(doc_root);
//     session * curr_sess = new session(io_service, r);

//     boost::system::error_code error;
//     int ret = s->handle_accept(curr_sess,error);

//     EXPECT_EQ(ret,0);
// }

// TEST(ServerHandleTest, Server) {
//     boost::asio::io_service io_service;
//     MockSession session(io_service);
//     short port = 8080;
//     server * s = nullptr;
//     s = new server(session.io_service, port);

//     boost::system::error_code error;
//     int ret = s->handle_accept(curr_sess,error);

//     EXPECT_CALL(session,start()).Times(testing::AtLeast(1));
// }