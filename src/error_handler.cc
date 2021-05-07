#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "reply.h"
#include "request.h"
#include "error_handler.h"
#include "config_parser.h"


namespace http {
namespace server {

error_handler::error_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{}

void error_handler::handle_request(const request& req, reply& rep) 
{
    rep = reply::stock_reply(reply::not_found);
    return;
}


} /// namespace server
} /// namespace http