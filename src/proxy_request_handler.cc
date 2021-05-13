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
    
    NginxConfigStatement * port_config_statement = NginxConfigParser::find_statement("port", &handler_config);
    host_port_ = std::stoi(NginxConfigParser::parse_string(port_config_statement->tokens_[1]));
}

http::response<http::string_body> proxy_request_handler::handle_request(
    const http::request<http::string_body>& request)
{
    http::response<http::string_body> response;
    
    // TODO replace fake response with call from server's response
    std::string req_URI = form_URI(request);
    BOOST_LOG_TRIVIAL(info) << "Proxy URI: " << req_URI;

    boost::asio::ip::tcp::iostream stream(host_name_, "http");
    stream << "GET " << req_URI << " HTTP/1.1\r\n";
    stream << "Host: " << host_name_ << "\r\n";
    stream << "Accept: */*\r\n";
    stream << "Connection: close\r\n\r\n";
    stream.flush();

    std::ostringstream ss;
    ss << stream.rdbuf();
    std::string result = ss.str();

    int headers_end = result.find("\r\n\r\n");
    std::string headers;
    
    if (headers_end != std::string::npos) {
        headers = result.substr(0, headers_end + 4);
        result = result.substr(headers_end + 4);
    }
        
    int code = get_code(headers);
    BOOST_LOG_TRIVIAL(debug) << "Response is: " << code;
    while (code >= 300 && code <= 399) {
        std::string new_host;
        std::string new_path;
        bool success = get_new_request(headers, new_host, new_path);
        if (!success) return response;
        BOOST_LOG_TRIVIAL(debug) << "300 redirect | Host: " << new_host
                                 << " | Path: " << new_path;
        boost::asio::ip::tcp::iostream redir_stream(new_host, "http");
        redir_stream << "GET " << new_path << " HTTP/1.1\r\n";
        redir_stream << "Host: " << new_host << "\r\n";
        redir_stream << "Accept: */*\r\n";
        redir_stream << "Connection: close\r\n\r\n";
        redir_stream.flush();

        std::ostringstream rss;
        rss << redir_stream.rdbuf();
        std::string redir_result = rss.str();

        headers_end = redir_result.find("\r\n\r\n");
        if (headers_end != std::string::npos) {
            headers = redir_result.substr(0, headers_end + 4);
            result = redir_result.substr(headers_end + 4);
        }
        
        code = get_code(headers);
        BOOST_LOG_TRIVIAL(debug) << "Response is: " << code;
    }
    
    response.body() += result;
    response.set("Content-Type", "text/plain");
    response.prepare_payload();

    BOOST_LOG_TRIVIAL(info) << "Prepared Proxy Request Response";
    
    return response;
}


bool proxy_request_handler::get_new_request(
    std::string headers, std::string& new_host, std::string& new_path) {
    bool success = false;
    std::string space = " ";
    std::string CRLF = "\r\n";
    
    std::string line;
    std::string location;
    while(true){
        if (headers.length() == 0) break;
        std::string line = headers.substr(0, headers.find(CRLF));
        std::string token = line.substr(0, headers.find(space));
        BOOST_LOG_TRIVIAL(trace) << "Examining line in proxy handler received header: " << line;
        if (token.find("location:") != std::string::npos ||
            token.find("Location:") != std::string::npos) {
            line.erase(0, std::string("location:").length());
            while(line[0] == ' ') line.erase(0,1);
            while(line.find(" ") != std::string::npos) line.erase(line.find(" "));
            success = true;
            location = line;
            break;
        }
        else headers.erase(0, line.length() + 2);
    }
    if (location.find("://") != std::string::npos) location.erase(0, location.find("://") + 3);
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

int proxy_request_handler::get_code(std::string headers){
    std::string space = " ";
    std::string CRLF = "\r\n";
    int code = 200;

    std::string token;
    int token_len;
    bool found_HTTP = false;
    for (int i = 0; i < 20; i++){
        std::string token_space = headers.substr(0, headers.find(space));
        std::string token_CRLF = headers.substr(0, headers.find(CRLF));
        
        if (token_space.length() < token_CRLF.length()) token_len = token_space.length();
        else token_len = token_CRLF.length();
        
        token = headers.substr(0, token_len);
        headers.erase(0, token.length() + 1);

        if (token.find("HTTP") != std::string::npos) {
            found_HTTP = true;
            continue;
        }
        if (found_HTTP){
            found_HTTP = false;
            try {
                code = std::stoi(token);
                break;
            } catch (const std::invalid_argument& ia) {
                BOOST_LOG_TRIVIAL(warning) << "Could not convert header token to status code";
            }
        }
        
    }
    return code;
}
