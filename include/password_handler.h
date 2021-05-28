#ifndef PASSWORD_CHECKER_HPP
#define PASSWORD_CHECKER_HPP

#include <string>

#include "request_handler.h"

class password_handler : public request_handler
{
public:
  /// Construct with a directory containing files to be served.
  explicit password_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

  std::string get_name();

private:
  std::string base_path_;
  std::string password = "";
};

#endif // PASSWORD_CHECKER_HPP