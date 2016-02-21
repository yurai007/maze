#include <iostream>
#include <csignal>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "../common/byte_buffer.hpp"
#include "../common/message_dispatcher.hpp"

namespace networking
{

server::server(short port)
    : m_io_service(),
      acceptor(m_io_service, tcp::endpoint(tcp::v4(), port)),
      m_signals(m_io_service)
{
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
   #if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
   #endif

    m_signals.async_wait([this](auto, auto) { this->stop();});
    register_handler_for_listening();
}

void server::add_dispatcher(std::shared_ptr<message_dispatcher> dispatcher)
{
    m_dispatcher = dispatcher;
}

void server::run()
{
    m_io_service.run();
}

void server::stop()
{
    m_io_service.stop();
    handle_stop();
}

void server::remove_connection(std::shared_ptr<connection> connection_)
{
    connections.erase(connection_);
    logger_.log("server: connection with id = %d was removed",
                connection_->get_socket().native_handle());
    connection_->stop();
}

void server::send_on_current_connection(const serialization::byte_buffer
                                        &data)
{
    current_connection->send(data);
}

void server::dispatch_msg_from_buffer(serialization::byte_buffer
                                      &buffer)
{
    m_dispatcher->dispatch_msg_from_buffer(buffer);
}

io_service &server::get_io_service()
{
    return m_io_service;
}

void server::register_handler_for_listening()
{
    logger_.log("server: start listening");
    auto new_connection = std::make_shared<connection>(m_io_service, *this);
    connections.insert(new_connection);

    acceptor.async_accept(
                new_connection->get_socket(),
                [this, new_connection](auto error){ this->handle_accept(new_connection, error);});
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

void server::handle_stop()
{
    logger_.log("server: stopped listening");
    acceptor.close();

    for (auto &connection : connections)
    {
        connections.erase(connection);
        connection->stop();
    }
    connections.clear();
}

}
