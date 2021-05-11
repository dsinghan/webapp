#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "proxy_request_handler.h"

proxy_request_handler::proxy_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{
  NginxConfigStatement * host_config_statement = NginxConfigParser::find_statement("host", &handler_config);
  host_name_ = NginxConfigParser::parse_string(host_config_statement->tokens_[1]);

  NginxConfigStatement * port_config_statement = NginxConfigParser::find_statement("port", &handler_config);
  host_port_ = std::stoi(NginxConfigParser::parse_string(port_config_statement->tokens_[1]));
}

boost::beast::http::response<boost::beast::http::string_body> proxy_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
  boost::beast::http::response<boost::beast::http::string_body> response;

  // TODO replace fake response with call from server's response
  response.body() += std::string("Place Holder for Request");
  response.set("Content-Type", "text/plain");
  response.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Prepared Proxy Request Response";

  return response;
}
