#include "gtest/gtest.h"
#include "session.h"
#include "gmock/gmock.h"

using ::testing::_;

TEST(TestReadGood, SessionTest) {
    boost::asio::io_service io_service;
    session * s = new session(io_service);
    boost::system::error_code error;
    char text[] = "Hello World";
    strncpy(s->data_,text,strlen(text));


    int ret = s->handle_read(error,strlen(text));

    EXPECT_EQ(ret,0);

}

TEST(TestWriteGood, SessionTest) {
    boost::asio::io_service io_service;
    session * s = new session(io_service);
    boost::system::error_code error;
    
    int ret = s->handle_write(error);

    EXPECT_EQ(ret,0);

}

TEST(TestReadBad, SessionTest) {
    boost::asio::io_service io_service;
    session * s = new session(io_service);
    boost::system::error_code error;
    char text[] = "Hello World";
    strncpy(s->data_,text,strlen(text));
    error.assign(1, boost::system::system_category());

    int ret = s->handle_read(error,strlen(text));


    EXPECT_EQ(ret,1);

}

TEST(TestWriteBad, SessionTest) {
    boost::asio::io_service io_service;
    session * s = new session(io_service);
    boost::system::error_code error;
    error.assign(1, boost::system::system_category());

    int ret = s->handle_write(error);

    EXPECT_EQ(ret,1);

}

TEST(TestSocket, SessionTest) {
    boost::asio::io_service io_service;
    session * s = new session(io_service);
    
    tcp::socket * sock = &s->socket();
    int ret = (&s->socket_ == sock);

}


