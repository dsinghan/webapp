#ifndef SESSION_H
#define SESSION_H

#include <cstdlib>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/bind.hpp>

#include "request_handler.h"

class session
{
public:
  session(
    boost::asio::io_service& io_service,
    std::map<std::string, request_handler*> locations,
    std::map<std::pair<std::string, int>, int> * request_results
  );

  //returns the server socket.
  boost::asio::ip::tcp::socket& socket();

  //gets the URL path and determines the first possible location of the request handler
  std::string determine_path(const boost::beast::http::request<boost::beast::http::string_body>& req);

  void start();

  void async_read();
  // private:
  int handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);

  static std::mutex request_results_lock_; 

  //removes the trailing path of the URL request. For example /static1/index.html -> /static1
  std::string remove_path_extension(std::string path);

  int handle_write(const boost::system::error_code& error);

  boost::asio::ip::tcp::socket socket_;
  enum { max_length = 8192 };
  char data_[max_length];

  std::map<std::string, request_handler*> locations_;
  std::map<std::pair<std::string, int>, int> * request_results_; // Reference member so server's map is updated by all sessions

  boost::beast::http::response<boost::beast::http::string_body> response_;

  boost::beast::http::request<boost::beast::http::string_body> request_;
};

#endif
