#ifndef ERROR_HTTP_REQUEST_HANDLER_HPP
#define ERROR_HTTP_REQUEST_HANDLER_HPP

#include "request_handler.h"

namespace http {
namespace server {

class error_handler : public request_handler 
{
public:
    explicit error_handler(std::string handler_location, const NginxConfig & handler_config);

    boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

};

} // namespace server
} // namespace http

#endif
