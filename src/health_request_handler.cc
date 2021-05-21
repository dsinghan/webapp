#include <string>

#include <boost/log/trivial.hpp>

#include "health_request_handler.h"

health_request_handler::health_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{}

boost::beast::http::response<boost::beast::http::string_body> health_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
    BOOST_LOG_TRIVIAL(warning) << "Producing Health Response";
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, request.version()};
    res.set(boost::beast::http::field::content_type, "text/plain");
    res.body() = "OK";
    res.prepare_payload();
    return res;
}

std::string health_request_handler::get_name() {
  return "Health Request Handler";
}