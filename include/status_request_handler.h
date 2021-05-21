#ifndef STATUS_REQUEST_HANDLER_HPP
#define STATUS_REQUEST_HANDLER_HPP

#include "request_handler.h"

class status_request_handler : public request_handler
{
public:
    explicit status_request_handler(std::string handler_location, const NginxConfig & handler_config);

    boost::beast::http::response<boost::beast::http::string_body> handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request);

    std::string get_name();

    void set_request_results(std::map<std::pair<std::string, int>, int> * request_results);

    void set_handlers_url_map(std::map<std::string, std::vector<std::string> > handlers_url_map);

private:
    std::map<std::pair<std::string, int>, int> * request_results_;
    std::map<std::string, std::vector<std::string> > handlers_url_map_;
};

#endif
