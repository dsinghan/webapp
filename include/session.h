#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "reply.h"
#include "request.h"
#include "static_request_handler.h"
#include "echo_request_handler.h"
#include "request_parser.h"

//using boost::asio::ip::tcp;

class session
{
public:
  session(
    boost::asio::io_service& io_service,
    std::map<std::string, http::server::request_handler*> locations
  );

  boost::asio::ip::tcp::socket& socket();

  std::string determine_path(http::server::request req);

  void start();

  void async_read();
// private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  int handle_write(const boost::system::error_code& error);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];

  /// The incoming request.
  http::server::request request_;

  /// The parser for the incoming request.
  http::server::request_parser request_parser_;

  /// The reply to be sent back to the client.
  http::server::reply reply_;

  std::map<std::string, http::server::request_handler*> locations_;
};

#endif