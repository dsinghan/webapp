#ifndef ECHO_HTTP_REQUEST_HANDLER_HPP
#define ECHO_HTTP_REQUEST_HANDLER_HPP

#include <string>
#include "request_handler.h"

namespace http {
namespace server {

class echo_request_handler : public request_handler
{
public:
  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);
};

} // namespace server
} // namespace http

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP