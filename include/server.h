#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "request_handler.h"

//using boost::asio::ip::tcp;

#include "session.h"

class server
{
public:
  server(boost::asio::io_service& io_service, short port, const std::string& doc_root);
  int handle_accept(session* new_session,
      const boost::system::error_code& error);

private:
  void start_accept();

  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  http::server::request_handler handler_;
};

#endif