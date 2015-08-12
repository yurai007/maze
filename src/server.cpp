#include <iostream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.h"

namespace networking
{

server::server(short port)
    : m_io_service(),
      acceptor(m_io_service, tcp::endpoint(tcp::v4(), port))
{
    register_handler_for_listening();
}

void server::run()
{
    m_io_service.run();
}

void server::remove_connection(std::shared_ptr<connection> connection_)
{
    connections.erase(connection_);
    logger_.log("server: connection was removed");
}

void server::register_handler_for_listening()
{
    logger_.log("server: start listening");
    auto new_connection = std::make_shared<connection>(m_io_service, *this);
    connections.insert(new_connection);

    acceptor.async_accept(
                new_connection->get_socket(),
                boost::bind(&server::handle_accept, this, new_connection, placeholders::error));
}

void server::handle_accept(std::shared_ptr<connection> new_connection,
                           const boost::system::error_code& error)
{
    if (!error)
    {
        std::string endpoint = boost::lexical_cast<std::string>(
                                    new_connection->get_socket().remote_endpoint());
        logger_.log("server: accepted next connection from %s", endpoint.c_str());
        new_connection->start();
    }
    else
    {
        logger_.log("server: connection accepting failed");
        new_connection.reset();
    }
    register_handler_for_listening();
}

}