#ifndef STATIC_REQUEST_HANDLER_HPP
#define STATIC_REQUEST_HANDLER_HPP

#include <string>
#include "request_handler.h"

namespace http {
namespace server {

// struct reply;
// struct request;

class static_request_handler : public request_handler
{
public:
  /// Construct with a directory containing files to be served.
  // explicit static_request_handler(const std::string& doc_root);
  explicit static_request_handler();

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep, std::string base_path);

private:
  /// The directory containing the files to be served.
  // std::string doc_root_;
};

} // namespace server
} // namespace http

#endif // STATIC_REQUEST_HANDLER_HPP