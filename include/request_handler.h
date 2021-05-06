//
// request_handler.h
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HANDLER_HPP
#define HTTP_REQUEST_HANDLER_HPP

#include <string>

// #include "config_parser.h"
class NginxConfig;

namespace http {
namespace server {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler
{
public:
  // Prevent copying and copy-constructing
  request_handler(const request_handler&) = delete;
  request_handler& operator=(const request_handler&) = delete;

  explicit request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  virtual void handle_request(const request& req, reply& rep) = 0;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);

private:
  std::string handler_location_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HANDLER_HPP