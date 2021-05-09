#ifndef ECHO_HTTP_REQUEST_HANDLER_HPP
#define ECHO_HTTP_REQUEST_HANDLER_HPP

#include <string>

#include "request_handler.h"

class echo_request_handler : public request_handler
{
public:
  explicit echo_request_handler(std::string handler_location, const NginxConfig & handler_config);
  
  /// Handle a request and produce a reply.
  boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);
};

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP