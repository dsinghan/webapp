## Logging Notes
- When adding new source files, make sure to use `#include <boost/log/trivial.hpp>`
- For each executable in `CMakeLists.txt`, make sure to include `Boost::log_setup` and `Boost::log` in `target_link_libraries`
