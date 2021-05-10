#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/log/trivial.hpp>

#include "config_parser.h"
#include "static_request_handler.h"

static_request_handler::static_request_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config)
{
  NginxConfigStatement * path_config_statement = NginxConfigParser::find_statement("root", &handler_config);
  base_path_ = NginxConfigParser::parse_string(path_config_statement->tokens_[1]);
}

boost::beast::http::response<boost::beast::http::string_body> static_request_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{
  std::string request_path;

  //Get the request of the path, without the request handler portion.
  request_path = std::string(request.target()).substr(handler_location_.length());

  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/')
  {
    request_path += "index.html";
  }

  BOOST_LOG_TRIVIAL(info) << "Request Path: " << request_path;

  // Determine the file extension.
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
    {
      extension = request_path.substr(last_dot_pos);
      BOOST_LOG_TRIVIAL(debug) << "Got extension: " << extension;
    }

  // Append base path to request path and pen the file to send back.
  std::string full_path = base_path_ + request_path;
  BOOST_LOG_TRIVIAL(info) << "Searching for file: " << full_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is)
  {
    BOOST_LOG_TRIVIAL(warning) << "Producing 404 Not Found";
    boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, request.version()};
    res.set(boost::beast::http::field::content_type, "text/html");
    res.body() = "404 Not Found";
    res.prepare_payload();
    return res;
  }

  // Create and fill a response object and return.
  boost::beast::http::response<boost::beast::http::string_body> response_;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    response_.body().append(buf, is.gcount());
  response_.set("Content-Type", mime_type(extension));
  response_.prepare_payload();

  BOOST_LOG_TRIVIAL(info) << "Prepared Static Response";

  return response_;
}

std::string static_request_handler::mime_type(std::string ext)
{
    using boost::beast::iequals;
    
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    // if(iequals(ext, ".php"))  return "text/html";
    // if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    // if(iequals(ext, ".js"))   return "application/javascript";
    // if(iequals(ext, ".json")) return "application/json";
    // if(iequals(ext, ".xml"))  return "application/xml";
    // if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    // if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    // if(iequals(ext, ".bmp"))  return "image/bmp";
    // if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    // if(iequals(ext, ".tiff")) return "image/tiff";
    // if(iequals(ext, ".tif"))  return "image/tiff";
    // if(iequals(ext, ".svg"))  return "image/svg+xml";
    // if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}