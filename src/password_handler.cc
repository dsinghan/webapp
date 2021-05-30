#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/log/trivial.hpp>

#include "config_parser.h"
#include "password_handler.h"

password_handler::password_handler(std::string handler_location, const NginxConfig & handler_config)
  : request_handler(handler_location, handler_config) 
  {

    NginxConfigStatement * path_config_statement = NginxConfigParser::find_statement("root", &handler_config);
    base_path_ = NginxConfigParser::parse_string(path_config_statement->tokens_[1]);

    std::string path = "/password.txt";
    
    // File that contains the password to our chat room
    std::string full_path = base_path_ + path;
    BOOST_LOG_TRIVIAL(info) << "Searching for file: " << full_path;
    std::ifstream inFile;
    inFile.open(full_path.c_str());


    if (!inFile) {
        BOOST_LOG_TRIVIAL(warning) << "File doesn't exist";
    }

    else {
        while (!inFile.eof()) {
            getline(inFile,password);
        }

        inFile.close();
    }
  }

boost::beast::http::response<boost::beast::http::string_body> password_handler::handle_request(const boost::beast::http::request<boost::beast::http::string_body>& request)
{

  std::string userPassword = "";

  for(auto const& field : request) {
    if ( std::string(to_string(field.name())) == "Security-Scheme") {
      userPassword = field.value().to_string();
      break;
    }
  }

  // Checks if user password is equal to actual password
  bool val = (userPassword == password);

  boost::beast::http::response<boost::beast::http::string_body> response;

  // Append the request headers to response body.
  if (val) {
    response.body() = "True";
  }
  else {
    response.body() = "False";
  }

  // Prepare response headers and return response object.
  response.set("Content-Type", "text/plain");
  response.prepare_payload();


  return response;
}

std::string password_handler::get_name() {
  return "Password Handler";
}