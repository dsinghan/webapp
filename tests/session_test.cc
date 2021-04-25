#include "gtest/gtest.h"
#include "session.h"

// //Test a good handle_read input
// TEST(TestReadGood, SessionTest) {
//     boost::asio::io_service io_service;
//     session * s = new session(io_service);
//     boost::system::error_code error;
//     char text[] = "Hello World";
//     strncpy(s->data_,text,strlen(text));


//     int ret = s->handle_read(error,strlen(text));

//     EXPECT_EQ(ret,0);

// }


// //Test a good handle write input
// TEST(TestWriteGood, SessionTest) {
//     boost::asio::io_service io_service;
//     session * s = new session(io_service);
//     boost::system::error_code error;
    
//     int ret = s->handle_write(error);

//     EXPECT_EQ(ret,0);

// }

// //Tests to make sure errors are handled properly in handle_read
// TEST(TestReadBad, SessionTest) {
//     boost::asio::io_service io_service;
//     session * s = new session(io_service);
//     boost::system::error_code error;
//     char text[] = "Hello World";
//     strncpy(s->data_,text,strlen(text));
//     error.assign(1, boost::system::system_category());

//     int ret = s->handle_read(error,strlen(text));


//     EXPECT_EQ(ret,1);

// }

// //Tests to make sure errors are handled properly in handle_write
// TEST(TestWriteBad, SessionTest) {
//     boost::asio::io_service io_service;
//     session * s = new session(io_service);
//     boost::system::error_code error;
//     error.assign(1, boost::system::system_category());

//     int ret = s->handle_write(error);

//     EXPECT_EQ(ret,1);

// }

// //Tests to make sure proper socket is returned in socket function
// TEST(TestSocket, SessionTest) {
//     boost::asio::io_service io_service;
//     session * s = new session(io_service);
    
//     tcp::socket * sock = &s->socket();
//     int ret = (&s->socket_ == sock);

// }


