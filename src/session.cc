#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/log/trivial.hpp>
#include <typeinfo>

#include "session.h"

std::mutex session::request_results_lock_;

session::session(
  boost::asio::io_service& io_service,
  std::map<std::string, request_handler*> locations,
  std::map<std::pair<std::string, int>, int> * request_results)
  : socket_(io_service), locations_(locations), request_results_(request_results) {}

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
    return "/";
  }

  // Request path must be absolute and not contain "..".
  if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != boost::beast::string_view::npos)
  {
    return "/";
  }

  BOOST_LOG_TRIVIAL(debug) << "Request Path: " << request_path;

  // Extract first potential request handler extension
  //Subsequent potential paths will be obtained by calling remove_path_extension function in handle_read
  int last_slash_pos = request_path.find_last_of('/');

  //path only has one "/"
  if (last_slash_pos == 0) {
    return request_path;
  }
  //No '/' found
  if (last_slash_pos == -1) {
    return "/";
  }

  BOOST_LOG_TRIVIAL(debug) << "Val: " << last_slash_pos;

  std::string path;
  if (last_slash_pos == std::string::npos) {
    path = request_path;
    BOOST_LOG_TRIVIAL(debug) << "Attempting Path: " << path;
  }
  else {
    path = request_path.substr(0, last_slash_pos);
    BOOST_LOG_TRIVIAL(debug) << "Attempting Path: " << path;
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

  BOOST_LOG_TRIVIAL(debug) << "Received request from " << socket_.remote_endpoint().address();

  boost::beast::http::request_parser<boost::beast::http::string_body> parser_;
  boost::system::error_code ec;
  std::string path;
  request_handler * selected_request_handler;

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
    path = determine_path(request_);

    //Looks for the longest prefix that matches a request handler name
    //If not found, remove trailing extension and try again
    while (locations_.find(path) == locations_.end()) {
      path = remove_path_extension(path);
    }

    //get the request handler object for the matching request handler
    selected_request_handler = locations_.find(path)->second;


    BOOST_LOG_TRIVIAL(debug) << "Handling request with path: " << path;
    response_ = selected_request_handler->handle_request(request_);

    // TODO: Is this the correct path?
    std::string url = std::string(request_.target());
    request_results_lock_.lock();
    (*request_results_)[std::make_pair(url, response_.result_int())]++;
    request_results_lock_.unlock();
    BOOST_LOG_TRIVIAL(debug) << "Updated " << url << ", " << response_.result_int() << ". It's now: " << (*request_results_)[std::make_pair(url, response_.result_int())];
  }

  // Write response to client.
  BOOST_LOG_TRIVIAL(info) << "[Response Metrics] [Response Code: " << response_.result_int() << 
                             "] [Request Path: " << std::string(request_.target()) << 
                             "] [Request IP: " << socket_.remote_endpoint().address() <<
                             "] [Handler Name: " << selected_request_handler->get_name() << "]";
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


//cut off trailing extension from path.
//For example: /static/hello/world would become /static/hello
std::string session::remove_path_extension(std::string path) {

  int last_slash_pos = path.find_last_of('/');

  if (last_slash_pos == -1 || last_slash_pos == 0) {
    path = "/";
  } else {
    path = path.substr(0, last_slash_pos);
  }

  BOOST_LOG_TRIVIAL(debug) << "Attempting path: " << path;

  return path;

}