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

Step 6: Return to the `powell-cat` directory   
`cd ..`

Step 7: Generate test code coverage report 
```
  $ mkdir build_coverage
  $ cd build_coverage
  $ cmake -DCMAKE_BUILD_TYPE=Coverage ..
  $ make coverage
```

Step 8: Run the server. But first, go back to the `powell-cat` directory.   
`build/bin/webserver conf/my_config`


# Source Code Explanation
We create a new `server` object when we initialize our application. This allows us to accept requests and wraps the rest of our application logic.  

The server accepts a config file in the format of an Nginx config file. It declares a config_parser object, which extracts the details from the config file when initializing the server. We use the "conf/my_config" file when starting our server. 

The "my_config" file contains the `locations` parameters, which redirects users to certain directories based on the url path they requested. This server uses a longest matching prefix algorithm to route the user to the directory that they are looking for. If the request url matches two `locations` of our config file, the algorithm will pick the matching location that is of the longest length.   

To add a new request handler in the conf/my_config file:   
```
  location URL_PATH REQUEST_HANDLER_NAME {
    root DIR_PATH;
  }
```   
* URL_PATH: the path the user provides
* REQUEST_HANDLER_NAME: the name of your new class
* DIR_PATH: the path to the directory
* Note: we only need the specify the "root" directory for StaticHandler locations
You can see examples in the conf/my_config file.    
 
The config parser instantiates a specific request handler for a specific path. Session determines what the path is, using an internal mapping to decide what type of request handler we should use. There are three types of request handler classes: static, echo, and error. Each one inherits from the "request_handler" base class. They each override the `handle_request` method, which returns a response depending on the url and type of handler.    
* Static request handler: serve static files to the user depending on the directory they are trying to access.   
* Echo request handler: return a copy of the request back to the user.   
* Error handler: return a 404 error to the user, signaling that the server could not find the resource that they were searching for.   

To add a new request handler class:   
1. Create a new .h file in the `include` directory. The class should inherit from "request_handler" and should override the `handle_request` method.   
2. Create a new .cc file in the `src` directory to implement the handle_request method, as well as other methods you desire.   
3. Add a new location in the `locations` parameter of the `conf/my_config` file.    
4. Extend the if-statement in the `create_handler` function of `src/config_parser.cc`.   



# Extra Notes
### Logging Notes
– The log outputs are written to "webserver_*.log" in the `powell-cat` directory
- When adding new source files, make sure to use `#include <boost/log/trivial.hpp>`
- For each executable in `CMakeLists.txt`, make sure to include `Boost::log_setup` and `Boost::log` in `target_link_libraries`

### References
– https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/ref/boost__beast__http__request.html
– https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/ref/boost__beast__http__response.html
– https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/ref/boost__beast__http__parser.html


