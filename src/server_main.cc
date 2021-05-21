// Adapted From:
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

#include "config_parser.h"
#include "request_handler.h"
#include "server.h"
#include "utils.h"

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

int main(int argc, char* argv[])
{
  init();

  try
  {
    if (argc != 2)
    {
      BOOST_LOG_TRIVIAL(fatal) << "Incorrect usage. Usage: webserver [CONFIG_FILE]";
      return 1;
    }

    boost::asio::io_service io_service;

    BOOST_LOG_TRIVIAL(debug) << "Parsing config file: " << argv[1];
    NginxConfigParser config_parser;
    NginxConfig config;

    bool parse_status = config_parser.Parse(argv[1], &config);
    if (!parse_status) {
      BOOST_LOG_TRIVIAL(fatal) << "Could not parse config file: " << argv[1];
      return 1;
    }

    int port = config_parser.extract_port(&config);
    if (port == -1) {
      BOOST_LOG_TRIVIAL(fatal) << "Could not extract port";
      return 1;
    }

    std::map<std::string, request_handler*> locations = config_parser.get_locations(&config);

    std::map<std::pair<std::string, int>, int> * request_results = config_parser.get_request_results();

    BOOST_LOG_TRIVIAL(info) << "Starting server with " << locations.size() << " locations on port " << port;
    server s(io_service, port, locations, request_results);
    io_service.run();
  }
  catch (std::exception& e)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
    return 1;
  }
}