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

#include <boost/beast/http.hpp>

class NginxConfig;

/// The common handler for all incoming requests.
class request_handler
{
public:
  // Prevent copying and copy-constructing
  request_handler(const request_handler&) = delete;
  request_handler& operator=(const request_handler&) = delete;

  explicit request_handler(std::string handler_location, const NginxConfig & handler_config);

  /// Handle a request and produce a reply.
  virtual boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request) = 0;
  virtual std::string get_name() = 0;

  /// Perform URL-decoding on a string. Returns false if the encoding was invalid.
  static bool url_decode(const std::string& in, std::string& out);

protected:
  std::string handler_location_;
};

#endif // HTTP_REQUEST_HANDLER_HPP