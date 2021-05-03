#include "echo_request_handler.h"
#include <boost/log/trivial.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include "mime_types.h"
#include "reply.h"
#include "request.h"
#include <iostream>
#include <vector>

namespace http {
namespace server {

void echo_request_handler::handle_request(const request& req, reply& rep)
{
  rep.status = reply::ok;
  rep.content.append(req.method);
  rep.content += " ";
  rep.content.append(req.uri);
  rep.content += " HTTP/";
  rep.content.append(std::to_string(req.http_version_major));
  rep.content += ".";
  rep.content.append(std::to_string(req.http_version_minor));
  rep.content += "\r\n";
  std::vector<header>::size_type sz = req.headers.size();
  for (unsigned i = 0; i < sz; i++) {
    rep.content.append(req.headers[i].name); 
    rep.content += ": ";
    rep.content.append(req.headers[i].value);
    rep.content += "\r\n";
  }
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "text/plain";
  return;
}

} // namespace server
} // namespace http