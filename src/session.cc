#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>

#include "session.h"

session::session(
  boost::asio::io_service& io_service,
  std::map<std::string, request_handler*> locations)
  : socket_(io_service), locations_(locations) {}

boost::asio::ip::tcp::socket& session::socket()
{
    return socket_;
}

void session::async_read() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void session::start()
{
    async_read();
}

std::string session::determine_path(const boost::beast::http::request<boost::beast::http::string_body>& req) {
  std::string request_path;

  // Decode url to path.
  if (!(locations_.begin()->second)->url_decode(std::string(req.target()), request_path))
  {
    return 0;
  }

  // Request path must be absolute and not contain "..".
  if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != boost::beast::string_view::npos)
  {
    return 0;
  }

  // Extract /static or /echo path and return.
  std::size_t second_slash_pos = request_path.find_first_of("/", 1);
  std::string path;
  if (second_slash_pos == std::string::npos) {
    path = request_path;
  }
  else {
    path = request_path.substr(0, second_slash_pos);
  }

  return path;
}

int session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (error) {
      delete this;
      return 1;
    }

  BOOST_LOG_TRIVIAL(info) << "Received request from " << socket_.remote_endpoint().address();

  boost::beast::http::request_parser<boost::beast::http::string_body> parser_;
  boost::system::error_code ec;

  // Parse buffer into request object.
  parser_.put(boost::asio::buffer(data_, max_length), ec);
  request_ = parser_.get();

  // If parser returns with an error, produce "bad request" response.
  if (ec.value() != 0) {
    BOOST_LOG_TRIVIAL(warning) << "Producing 400 Bad Request";
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::bad_request, request_.version()};
    res.set(boost::beast::http::field::content_type, "text/html");
    res.body() = "400 Bad Request";
    res.prepare_payload();
    response_ = res;
  }

  // If parser successfully parses, call appropriate handle_request.
  else {
    std::string path = determine_path(request_);
    request_handler * selected_request_handler;
    if (locations_.find(path) != locations_.end()) {
      selected_request_handler = locations_.find(path)->second;
    } else {
      selected_request_handler = locations_.find("/")->second;
    }
    BOOST_LOG_TRIVIAL(info) << "Handling request with path: " << path;
    response_ = selected_request_handler->handle_request(request_);
  }

  // Write response to client.
  boost::beast::http::async_write(socket_, response_, boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
  return 0;
}

int session::handle_write(const boost::system::error_code& error)
{
    
    if (!error)
    {
        async_read();
        return 0;
    }
    else
    {
        BOOST_LOG_TRIVIAL(fatal) << "ERROR";
        delete this;
        return 1;
    }
}