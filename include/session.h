#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "reply.h"
#include "request.h"
#include "request_handler.h"
#include "request_parser.h"

//using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, http::server::request_handler& handler);

  boost::asio::ip::tcp::socket& socket();

  void start();

  void async_read();
// private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  int handle_write(const boost::system::error_code& error);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];

  /// The handler used to process the incoming request.
  http::server::request_handler& request_handler_;

  /// The incoming request.
  http::server::request request_;

  /// The parser for the incoming request.
  http::server::request_parser request_parser_;

  /// The reply to be sent back to the client.
  http::server::reply reply_;
};

#endif