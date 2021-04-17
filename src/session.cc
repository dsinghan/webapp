#include "session.h"

session::session(boost::asio::io_service& io_service)
: socket_(io_service)
{
}

tcp::socket& session::socket()
{
    return socket_;
}

void session::async_read() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void session::start()
{
    async_read();
}

int session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
        boost::asio::streambuf response_buffer;
        std::ostream response_stream(&response_buffer);
        response_stream << "HTTP/1.1 200 OK\r\n";
        response_stream << "Content-Type: text/plain\r\n";
        response_stream << "Content-Length: " << bytes_transferred << "\r\n\r\n";
        response_stream << std::string(data_, data_ + bytes_transferred);

        boost::asio::async_write(socket_,
            response_buffer,
            boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
        
        return 0;
        
    }
    else
    {
        delete this;
        return 1;
    }
}

int session::handle_write(const boost::system::error_code& error)
{
    
    if (!error)
    {
        async_read();
        
        return 0;
    }
    else
    {
        delete this;
        return 1;
    }
}