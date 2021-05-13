#ifndef HTTP_CLIENT_HPP
#define HTTP_CLIENT_HPP

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class http_client
{
public:
    http::response<http::string_body> send_request(std::string req_uri, std::string host_name, std::string host_port); 
};

#endif // ECHO_HTTP_REQUEST_HANDLER_HPP
