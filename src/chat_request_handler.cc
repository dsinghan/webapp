#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/log/trivial.hpp>

#include "config_parser.h"
#include "chat_request_handler.h"

chat_request_handler::chat_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{
  NginxConfigStatement * path_config_statement = NginxConfigParser::find_statement("root", &handler_config);
  base_path_ = NginxConfigParser::parse_string(path_config_statement->tokens_[1]);
}

boost::beast::http::response<boost::beast::http::string_body> chat_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
  std::string request_path;

  //Get the request of the path, without the request handler portion.
  request_path = std::string(request.target()).substr(handler_location_.length());

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    request_path += "index.html";
  }
  else if (request_path.empty()) {
    request_path += "/index.html";
  }

  BOOST_LOG_TRIVIAL(debug) << "Request Path: " << request_path;

  // Append base path to request path and pen the file to send back.
  std::string full_path = base_path_ + request_path;
  BOOST_LOG_TRIVIAL(debug) << "Searching for file: " << full_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    BOOST_LOG_TRIVIAL(warning) << "Producing 404 Not Found";
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, request.version()};
    res.set(boost::beast::http::field::content_type, "text/html");
    res.body() = "404 Not Found";
    res.prepare_payload();
    return res;
  }

  // Create and fill a response object and return.
  boost::beast::http::response<boost::beast::http::string_body> response_;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    response_.body().append(buf, is.gcount());
  response_.set("Content-Type", "text/html");
  response_.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Producing Chat Response";

  return response_;
}

std::string chat_request_handler::get_name() {
  return "Chat Request Handler";
}