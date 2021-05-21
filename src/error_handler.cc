#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "error_handler.h"
#include "config_parser.h"

error_handler::error_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{}

boost::beast::http::response<boost::beast::http::string_body> error_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
    BOOST_LOG_TRIVIAL(warning) << "Producing 404 Not Found";
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, request.version()};
    res.set(boost::beast::http::field::content_type, "text/html");
    res.body() = "404 Not Found";
    res.prepare_payload();
    return res;
}

std::string error_handler::get_name() {
  return "Error Handler";
}