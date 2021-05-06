#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/log/trivial.hpp>

#include "config_parser.h"
#include "mime_types.h"
#include "reply.h"
#include "request.h"
#include "static_request_handler.h"

namespace http {
namespace server {

static_request_handler::static_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{
  NginxConfigStatement * path_config_statement = NginxConfigParser::find_statement("root", &handler_config);
  base_path_ = NginxConfigParser::parse_string(path_config_statement->tokens_[1]);
}

void static_request_handler::handle_request(const request& req, reply& rep)
{
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  BOOST_LOG_TRIVIAL(info) << "Request Path: " << request_path;

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/'
      || request_path.find("..") != std::string::npos)
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Extract /static or /echo path
  std::size_t second_slash_pos = request_path.find_first_of("/", 1);
  std::string path;
  if (second_slash_pos == std::string::npos) {
    path = request_path;
    request_path = "/";
  }
  else {
    path = request_path.substr(0, second_slash_pos);
    request_path = request_path.substr(second_slash_pos);
  }

  BOOST_LOG_TRIVIAL(info) << "Path: " << path;

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    BOOST_LOG_TRIVIAL(debug) << "Got extension: " << extension;
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = base_path_ + request_path;
  BOOST_LOG_TRIVIAL(info) << "Searching for file: " << full_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    rep = reply::stock_reply(reply::not_found);
    return;
  }

  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = mime_types::extension_to_type(extension);
  BOOST_LOG_TRIVIAL(info) << "Prepared Static Response";
}

} // namespace server
} // namespace http
