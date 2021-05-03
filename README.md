## Logging Notes
- When adding new source files, make sure to use `#include <boost/log/trivial.hpp>`
- For each executable in `CMakeLists.txt`, make sure to include `Boost::log_setup` and `Boost::log` in `target_link_libraries`

## References
- header.h, mime_types.h, reply.h, request_handler.h, request_parser.h, request.h, mime_types.cc, reply.cc, request_handler.cc, request_parser.cc 
  are all adapted from the Boost C++ Examples at https://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html