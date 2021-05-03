#include "session.h"
#include <string>
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "request_parser.h"
#include <boost/log/trivial.hpp>

using boost::asio::ip::tcp;
session::session(
  boost::asio::io_service& io_service,
  // http::server::echo_request_handler& echo_request_handler,
  // http::server::static_request_handler& static_request_handler,
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
    if (!error)
    {
        BOOST_LOG_TRIVIAL(info) << "Received request from " << socket_.remote_endpoint().address();

        // Parse data into request object
        http::server::request_parser::result_type result;
        std::tie(result, std::ignore) = request_parser_.parse(
              request_, data_, data_ + bytes_transferred);

        // If good HTTP request, produce appropriate reply
        if (result == http::server::request_parser::good)
          {
            std::string path = determine_path(request_);
            BOOST_LOG_TRIVIAL(info) << "Got good request with path: " << path;
            
            // Static response
            if (locations_.find(path) != locations_.end() && path != "/echo") {
              std::string base_path;
              BOOST_LOG_TRIVIAL(info) << "Handling static request";

              // base_path = locations_[path];
              // static_request_handler_.handle_request(request_, reply_, base_path);
              locations_.find(path)->second->handle_request(request_, reply_);
            }
            
            // Echo response
            else {
              BOOST_LOG_TRIVIAL(info) << "Handling echo request";

              locations_.find("/echo")->second->handle_request(request_, reply_);
            }

            boost::asio::async_write(socket_,
                  reply_.to_buffers(),
                  boost::bind(&session::handle_write, this,
                  boost::asio::placeholders::error));
          }

        // If bad HTTP request, produce "400 Bad Request" reply
        else if (result == http::server::request_parser::bad)
          {
            BOOST_LOG_TRIVIAL(warning) << "Producing 400 Bad Request";
            reply_ = http::server::reply::stock_reply(http::server::reply::bad_request);
            boost::asio::async_write(socket_,
                reply_.to_buffers(),
                boost::bind(&session::handle_write, this,
                boost::asio::placeholders::error));
          }

        // Default echo
        else
          {
            boost::asio::async_write(socket_,
                boost::asio::buffer(data_, bytes_transferred),
                boost::bind(&session::handle_write, this,
                boost::asio::placeholders::error));
          }
        return 0;

    }
    else
    {
        delete this;
        return 1;
    }
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