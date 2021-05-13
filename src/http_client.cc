#include <boost/beast/core.hpp>

#include "http_client.h"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;    

http::response<http::string_body> http_client::send_request(std::string req_uri, std::string host_name, std::string host_port){
	// FROM: https://www.boost.org/doc/libs/1_70_0/libs/beast/doc/html/beast/quick_start/http_client.html
        
        net::io_context ioc;
	// These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host_name, host_port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);

        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, req_uri, 11};
        req.set(http::field::host, host_name);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::string_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};
	return res;
}
