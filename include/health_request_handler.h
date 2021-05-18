#ifndef HEALTH_HTTP_REQUEST_HANDLER_HPP
#define HEALTH_HTTP_REQUEST_HANDLER_HPP

#include "request_handler.h"

class health_request_handler : public request_handler
{
public:
    explicit health_request_handler(std::string handler_location, const NginxConfig & handler_config);

    boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

};

#endif
