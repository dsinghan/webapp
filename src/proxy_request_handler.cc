#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "proxy_request_handler.h"

proxy_request_handler::proxy_request_handler(
    std::string handler_location, const NginxConfig & handler_config)
    : request_handler(handler_location, handler_config)
{
    NginxConfigStatement * host_config_statement =
        NginxConfigParser::find_statement("host", &handler_config);
    host_name_ =
        NginxConfigParser::parse_string(host_config_statement->tokens_[1]);
    
    NginxConfigStatement * port_config_statement =
        NginxConfigParser::find_statement("port", &handler_config);
    host_port_ = NginxConfigParser::parse_string(port_config_statement->tokens_[1]);
}

proxy_request_handler::proxy_request_handler(
    std::string handler_location, const NginxConfig & handler_config, http_client http)
    : request_handler(handler_location, handler_config), http_(http)
{
    NginxConfigStatement * host_config_statement =
        NginxConfigParser::find_statement("host", &handler_config);
    host_name_ =
        NginxConfigParser::parse_string(host_config_statement->tokens_[1]);

    NginxConfigStatement * port_config_statement =
        NginxConfigParser::find_statement("port", &handler_config);
    host_port_ = NginxConfigParser::parse_string(port_config_statement->tokens_[1]);
}

http::response<http::string_body> proxy_request_handler::handle_request(
    const http::request<http::string_body>& request)
{
    std::string req_URI = form_URI(request);
    BOOST_LOG_TRIVIAL(info) << "Proxy URI: " << req_URI;
    // Send the initial request
    auto response = http_.send_request(req_URI, host_name_, host_port_);

    // Keep track of the status code (200 OK, 404 not found, etc.)
    // In case we need to submit another request for a redirect
    unsigned int code = static_cast<unsigned>(response.result());
    BOOST_LOG_TRIVIAL(debug) << "Response is: " << code;

    int num_redirects = 0;
    while (300 <= code && code <= 308) {
        if (num_redirects >= 20){ // 20 is the standard redirect limit from chrome
            response.version(11);
            response.result(http::status::not_found);
            response.body() = "404 Resource Not Found";
            response.prepare_payload();
            BOOST_LOG_TRIVIAL(info) << "Too many redirects, returning 404";
            return response;
        }
        num_redirects++;
        std::string new_host;
        std::string new_path;
	std::string protocol;
        get_new_request(response, new_host, new_path, protocol);
        BOOST_LOG_TRIVIAL(debug) << "300 redirect | Host: " << new_host
                                 << " | Path: " << new_path;
	if(protocol == "https"){
	  BOOST_LOG_TRIVIAL(debug) << "Received https redirect, returning redirect response";
	  return response;
	}
        // Send another request for the redirect target
        response = http_.send_request(new_path, new_host, host_port_);
        code = static_cast<unsigned>(response.result());
        BOOST_LOG_TRIVIAL(debug) << "Response is: " << code;
    }
    return response;
}


bool proxy_request_handler::get_new_request(
    http::response<http::string_body> response, std::string& new_host, std::string& new_path, std::string& protocol) {
    bool success = false;
    std::string location = std::string(response["Location"]);

    //remove the http:// part of the request if it exists
    if (location.find("://") != std::string::npos) {
	protocol = location.substr(0, location.find("://"));
	location.erase(0, location.find("://") + 3);
    }
    
    //split host to location before '/' if one exists
    if (location.find("/") == std::string::npos) {
        new_host = location;
        new_path = "/";
    }
    else {
        new_host = location.substr(0, location.find("/"));
        new_path = location.substr(location.find("/"));
    }
    return success;

}
std::string proxy_request_handler::form_URI(const http::request<http::string_body>& request) {
    std::string URI_to_req;
    //Take the intial request to find the correct uri to send the request to by removing the current server's path
    bool success = url_decode(std::string(request.target()), URI_to_req);
    if (!success) return "/";

    if (handler_location_.find_last_of('/') == handler_location_.length() - 1)
        handler_location_.pop_back();

    std::size_t prefix_pos = URI_to_req.find(handler_location_);
    if (prefix_pos == std::string::npos) return "/";
    URI_to_req.erase(0, handler_location_.length());
    if (URI_to_req.length() == 0) return "/";
    
    return URI_to_req;
}

std::string proxy_request_handler::get_name() {
    return "Proxy Request Handler";
}