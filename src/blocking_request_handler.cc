#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "blocking_request_handler.h"
#include "config_parser.h"

blocking_request_handler::blocking_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{}

boost::beast::http::response<boost::beast::http::string_body> blocking_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
    BOOST_LOG_TRIVIAL(warning) << "Blocking request handler is putting server to sleep temporarily";
    sleep(5);
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, request.version()};
    res.body() = "Successfully put server to sleep";
    res.prepare_payload();
    return res;
}

std::string blocking_request_handler::get_name() {
  return "Blocking Request Handler";
}