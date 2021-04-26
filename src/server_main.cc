// tt
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <csignal>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "session.h"
#include "server.h"
#include "config_parser.h"

using boost::asio::ip::tcp;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging() {
  logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

  logging::add_console_log(
    std::cerr,
    keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
  );

  logging::add_file_log(
    keywords::file_name = "webserver_%N.log",
    keywords::rotation_size = 10 * 1000 * 1000,  // 10 MB
    keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),  // Midnight
    keywords::auto_flush = true,  // For reference: http://boost-log.sourceforge.net/libs/log/doc/html/log/detailed/sink_backends.html
    keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
  );

  logging::core::get()->set_filter(
    logging::trivial::severity >= logging::trivial::debug
  );

  logging::add_common_attributes();
}


void signal_handler( int signum ) {
   BOOST_LOG_TRIVIAL(info) << "Interrupt signal (" << signum << ") received.\n";

   exit(0);
}

int main(int argc, char* argv[])
{
  init_logging();
  signal(SIGINT, signal_handler);
  BOOST_LOG_TRIVIAL(info) << "Logging initialized";

  try
  {
    if (argc != 2)
    {
      BOOST_LOG_TRIVIAL(fatal) << "Incorrect usage. Usage: webserver [CONFIG_FILE]";
      return 1;
    }

    boost::asio::io_service io_service;

    BOOST_LOG_TRIVIAL(info) << "Parsing config file: " << argv[1];
    NginxConfigParser config_parser;
    NginxConfig config;
    int port;
    std::string dir;

    // Extract port
    port = config_parser.extract_port(argv[1], &config);
    if (port == -1) {
      BOOST_LOG_TRIVIAL(fatal) << "Could not extract port";
      return 1;
    }

    // Extract locations mapping
    std::map<std::string, std::string> locations = config_parser.get_locations(&config);

    // for (auto const& x : locations)
    // {
    //     std::cout << x.first  // string (key)
    //               << ':' 
    //               << x.second // string's value
    //               << std::endl;
    // }

    BOOST_LOG_TRIVIAL(info) << "Starting server";

    server s(io_service, port, locations);

    io_service.run();
  }
  catch (std::exception& e)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Exception: " << e.what();
    return 1;
  }

  return 0;
}
