Welcome to powell-cat! :)

# What am I?
This is a C++ web server designed to facilitate the rapid development of various methods of handling requests. Depending on the path requested, the server currently serves static files or echoes back requests. If there is an error, we return a 404. Each of these three types of responses has its own "request_handler" class. 

# Build, Test, and Run
Step 1: Enter your base directory that contains the `powell-cat` and `tools` directories. 

Step 2: Start the docker container       
tools/env/start.sh -u USERNAME  -- -p    127.0.0.1:8080:8080   

Step 3: `cd powell-cat/build`   

Step 4: Build the web server   
`cmake .. && make`   

Step 5: Run the tests   
`make test`  

Step 6: Run the server. But first, go back to the `powell-cat` directory.   
`cd ..`    
`build/bin/webserver conf/my_config`


# Source Code Explanation
We create a new `server` object when we initialize our application. This allows us to accept requests and wraps the rest of our application logic.  

The server accepts a config file in the format of an Nginx config file. It declares a config_parser object, which extracts the details from the config file when initializing the server. We use the "conf/my_config" file when starting our server. It contains `locations` server parameters, which redirect users to certain directories based on their path (see below). The config parser decides which request handler we should use.

The server instantiates a new instance of the `session` class. The session handles our requests, determines which 


The `include` directory contains the header files.   
* config_parser.h
* echo_request_handler.h
* error_handler.h
* request_handler.h
* server.h
* session.h
* static_request_handler.h

# Extra Notes
### Logging Notes
- When adding new source files, make sure to use `#include <boost/log/trivial.hpp>`
- For each executable in `CMakeLists.txt`, make sure to include `Boost::log_setup` and `Boost::log` in `target_link_libraries`

### References
- header.h, mime_types.h, reply.h, request_handler.h, request_parser.h, request.h, mime_types.cc, reply.cc, request_handler.cc, request_parser.cc 
  are all adapted from the Boost C++ Examples at https://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html