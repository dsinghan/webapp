#ifndef PROXY_HTTP_REQUEST_HANDLER_HPP
#define PROXY_HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <boost/asio.hpp>

#include "config_parser.h"
#include "request_handler.h"

namespace http = boost::beast::http;

class proxy_request_handler : public request_handler
{
public:
  explicit proxy_request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  http::response<http::string_body> handle_request(const http::request<http::string_body>& request);

private:
  /// The url of the website to send a request to
  std::string host_name_;

    /// Formulate a proper HTTP request to the website
    std::string form_URI(const http::request<http::string_body>& request);

    /// Find the code in the response headers
    static int get_code(std::string headers);

    static bool get_new_request(
        std::string headers, std::string& new_host, std::string& new_path);

  /// The port number to issue requests to
  int host_port_;
};

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP
