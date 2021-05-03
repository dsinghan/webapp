#include "server.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port, std::map<std::string, http::server::request_handler*> locations)
: io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    // static_handler_(),
    // echo_handler_(),
    locations_(locations)
{
    start_accept();
}

void server::start_accept()
{
    session* new_session = new session(io_service_, locations_);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
            boost::asio::placeholders::error));
}

int server::handle_accept(session* new_session,
    const boost::system::error_code& error)
{
    if (!error)
    {
        new_session->start();
        start_accept();
        return 0;
    }
    else
    {
        delete new_session;
        start_accept();
        return 1;
    }

}