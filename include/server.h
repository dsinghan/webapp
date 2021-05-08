#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "static_request_handler.h"
#include "echo_request_handler.h"
#include "session.h"

class server
{
public:
  server(boost::asio::io_service& io_service, short port, std::map<std::string, request_handler*> locations);
  int handle_accept(session* new_session,
      const boost::system::error_code& error);

private:
  void start_accept();

  boost::asio::io_service& io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::map<std::string, request_handler*> locations_;
};

#endif