#ifndef PROXY_HTTP_REQUEST_HANDLER_HPP
#define PROXY_HTTP_REQUEST_HANDLER_HPP

#include <string>

#include "config_parser.h"
#include "request_handler.h"

class proxy_request_handler : public request_handler
{
public:
  explicit proxy_request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

private:
  /// The url of the website to send a request to
  std::string host_name_;

  /// The port number to issue requests to
  int host_port_;
};

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP
