#ifndef STATIC_REQUEST_HANDLER_HPP
#define STATIC_REQUEST_HANDLER_HPP

#include <string>

#include "request_handler.h"
#include "config_parser.h"

namespace http {
namespace server {

// struct reply;
// struct request;

class static_request_handler : public request_handler
{
public:
  /// Construct with a directory containing files to be served.
  explicit static_request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

private:
  /// The directory containing the files to be served.
  std::string base_path_;
};

} // namespace server
} // namespace http

#endif // STATIC_REQUEST_HANDLER_HPP