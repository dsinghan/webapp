#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "status_request_handler.h"

status_request_handler::status_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config), request_results_{}
{}

boost::beast::http::response<boost::beast::http::string_body> status_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
    if (request_results_ == nullptr) {
      BOOST_LOG_TRIVIAL(warning) << "No Request Results Available";
      boost::beast::http::response<boost::beast::http::string_body> response;
      response.set("Content-Type", "text/plain");
      response.body() = "No Request Results Available\n";
      response.prepare_payload();
      return response;
    }

    BOOST_LOG_TRIVIAL(info) << "Producing Status Report of size" << request_results_->size();

    boost::beast::http::response<boost::beast::http::string_body> response;

    // Construct the report
    response.body() = "Report:\n\n";

    response.body() += "Requests Received:\n";
    for (auto const & entry : *request_results_) {
      std::string path = entry.first.first;
      std::string status_str = std::to_string(entry.first.second);
      std::string count_str = std::to_string(entry.second);

      BOOST_LOG_TRIVIAL(info) << "New line: " << path << " " << status_str << " " << count_str;

      response.body() += path + ", " + status_str + ": " + count_str + "\n";
    }

    response.body() += "\n";

    response.body() += "Request Handlers:\n";
    for (auto const & entry : handlers_url_map_) {
      std::string handler_name = entry.first;
      std::vector<std::string> handler_urls = entry.second;

      response.body() += handler_name + "\n";
      for (auto const & url : handler_urls) {
        response.body() += "\t" + url + "\n";
      }
    }

    // Prepare response headers and return response object.
    response.set("Content-Type", "text/plain");
    response.prepare_payload();

    BOOST_LOG_TRIVIAL(info) << "Prepared Status Report";
    return response;
}

void status_request_handler::set_request_results(std::map<std::pair<std::string, int>, int> * request_results) {
    request_results_ = request_results;
}

void status_request_handler::set_handlers_url_map(std::map<std::string, std::vector<std::string> > handlers_url_map) {
    handlers_url_map_ = handlers_url_map;
}