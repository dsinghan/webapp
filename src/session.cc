#include "session.h"
#include <string>
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "request_parser.h"
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;
session::session(
  boost::asio::io_service& io_service,
  std::map<std::string, http::server::request_handler*> locations
)
: socket_(io_service), locations_(locations)
{

}

tcp::socket& session::socket()
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

std::string session::determine_path(http::server::request req) {
    // Decode url to path.
  std::string request_path;

  // accessing first request handler object to call url decode
  if (!(locations_.begin()->second)->url_decode(req.uri, request_path))
  {
    return 0;
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    return 0;
  }

  // Extract /static or /echo path
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

  http::server::request_parser::result_type result;
  request_parser_.reset();
  std::tie(result, std::ignore) = request_parser_.parse(request_, data_, data_ + bytes_transferred);

  if (result != http::server::request_parser::good) {
    BOOST_LOG_TRIVIAL(warning) << "Producing 400 Bad Request";
    reply_ = http::server::reply::stock_reply(http::server::reply::bad_request);
    boost::asio::async_write(socket_,
        reply_.to_buffers(),
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));
    return 0;
  }

  std::string path = determine_path(request_);
  http::server::request_handler * selected_request_handler;
  if (locations_.find(path) != locations_.end()) {
    selected_request_handler = locations_.find(path)->second;
  } else {
    selected_request_handler = locations_.find("/echo")->second;
  }
  BOOST_LOG_TRIVIAL(info) << "Handling request with path: " << path;
  selected_request_handler->handle_request(request_, reply_);

  boost::asio::async_write(socket_,
        reply_.to_buffers(),
        boost::bind(&session::handle_write, this,
        boost::asio::placeholders::error));

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
        delete this;
        return 1;
    }
}