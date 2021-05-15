#ifndef PROXY_HTTP_REQUEST_HANDLER_HPP
#define PROXY_HTTP_REQUEST_HANDLER_HPP

#include <string>
#include <boost/asio.hpp>

#include "config_parser.h"
#include "request_handler.h"
#include "http_client.h"

namespace http = boost::beast::http;

class proxy_request_handler : public request_handler
{
public:
  explicit proxy_request_handler(std::string handler_location, const NginxConfig & handler_config);
  explicit proxy_request_handler(std::string handler_location, const NginxConfig & handler_config, http_client http);
  /// Handle a request and produce a reply.
  http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
    /// Formulate a proper HTTP request to the website
    std::string form_URI(const http::request<http::string_body>& request);

private:
  /// The url of the website to send a request to
  std::string host_name_;
  http_client http_;

    static bool get_new_request(
        http::response<http::string_body>, std::string& new_host, std::string& new_path, std::string& protocol);

  /// The port number to issue requests to
    std::string host_port_;
};

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP
