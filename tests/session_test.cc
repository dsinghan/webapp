#include "gtest/gtest.h"
#include "session.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "config_parser.h"

class sessionTest : public ::testing::Test {
 protected:
    boost::asio::io_service io_service;
    NginxConfigParser parser;
    NginxConfig config;
    bool value = parser.extract_port("sessionConfig", &config);
    std::map<std::string, http::server::request_handler*> locations = parser.get_locations(&config);
    session * mySession = new session(io_service,locations);
    bool success;
};

//Test a good handle_read input
TEST_F(sessionTest, testGetLocation) {

    EXPECT_EQ(locations.size(),3);

}

// TEST_F(sessionTest, handleRead) {
    
//     boost::system::error_code error;
//     char text[] = "Hello World";
//     strncpy(mySession->data_,text,strlen(text));

//     int val = mySession->handle_read(error,12);

//     EXPECT_EQ(0,0);


// }

//Test a good handle write input
TEST_F(sessionTest, determinePath) {
    http::server::request req;
    req.uri = "/static1/index.html";
    std::string ret = mySession->determine_path(req);
    EXPECT_EQ("/static1",ret);

}

//Tests to make sure errors are handled properly in handle_read
TEST_F(sessionTest, SessionTest) {

    boost::system::error_code error;
    char text[] = "Hello World";
    strncpy(mySession->data_,text,strlen(text));
    error.assign(1, boost::system::system_category());

    int ret = mySession->handle_read(error,strlen(text));


    EXPECT_EQ(ret,1);

}

//Tests to make sure errors are handled properly in handle_write
TEST_F(sessionTest, testSession) {
    
    boost::system::error_code error;
    error.assign(1, boost::system::system_category());

    int ret = mySession->handle_write(error);

    EXPECT_EQ(ret,1);

}

//Tests to make sure proper socket is returned in socket function
TEST_F(sessionTest, testSocket) {
    
    boost::asio::ip::tcp::socket * sock = &mySession->socket();
    int ret = (&mySession->socket_ == sock);

}


