#include "echo_request_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include "mime_types.h"
#include "reply.h"
#include "request.h"
#include <iostream>

namespace http {
namespace server {

void echo_request_handler::handle_request(const request& req, reply& rep)
{
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path))
  {
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

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
  }
  else {
    path = request_path.substr(0, second_slash_pos);
  }

  // If path begins with /echo, serve echo responses
  // Otherwise, serve static files
  if (path == "/echo") {
    rep.content = "echo";
  }
  else {
    rep = reply::stock_reply(reply::not_found);
  }
  return;
}

} // namespace server
} // namespace http