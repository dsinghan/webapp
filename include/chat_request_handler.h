#ifndef CHAT_REQUEST_HANDLER_HPP
#define CHAT_REQUEST_HANDLER_HPP

#include <string>

#include "config_parser.h"
#include "request_handler.h"

class chat_request_handler : public request_handler
{
public:
  /// Construct with a directory containing files to be served.
  explicit chat_request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

  std::string get_name();

private:
  /// The directory containing the files to be served.
  std::string base_path_;

};

#endif // CHAT_REQUEST_HANDLER_HPP
