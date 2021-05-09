#include <csignal>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "utils.h"

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

  BOOST_LOG_TRIVIAL(info) << "Logging initialized";

}


void signal_handler( int signum ) {
   BOOST_LOG_TRIVIAL(info) << "Interrupt signal (" << signum << ") received.\n";
   exit(0);
}

void init() {
  init_logging();
  signal(SIGINT, signal_handler);
}