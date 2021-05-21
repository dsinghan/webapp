#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "echo_request_handler.h"

echo_request_handler::echo_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config) {}

boost::beast::http::response<boost::beast::http::string_body> echo_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
  boost::beast::http::response<boost::beast::http::string_body> response;

  // Append the request status line to response body.
  response.body() += std::string(request.method_string());
  response.body() += " ";
  response.body() += std::string(request.target());
  response.body() += " HTTP/";
  response.body() += std::to_string(request.version()/10);
  response.body() += ".";
  response.body() += std::to_string(request.version()%10);
  response.body() += "\r\n";

  // Append the request headers to response body.
  for(auto const& field : request) {
    if (field.name() == boost::beast::http::field::unknown) {
      continue;
    }
    response.body() += std::string(to_string(field.name()));
    response.body() += ": ";
    response.body() += std::string(request[field.name()]);
    response.body() += "\r\n";
  }

  // Prepare response headers and return response object.
  response.set("Content-Type", "text/plain");
  response.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Producing Echo Response";

  return response;
}

std::string echo_request_handler::get_name() {
  return "Echo Request Handler";
}