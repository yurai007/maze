#include <iostream>
#include <csignal>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.h"
#include "byte_buffer.hpp"
#include "message_dispatcher.hpp"

namespace networking
{

server::server(short port)
    : m_io_service(),
      acceptor(m_io_service, tcp::endpoint(tcp::v4(), port)),
      m_signals(m_io_service)
{
    // Register to handle the signals that indicate when the server should exit.
     // It is safe to register for the same signal multiple times in a program,
     // provided all registration for the specified signal is made through Asio.
     m_signals.add(SIGINT);
     m_signals.add(SIGTERM);
   #if defined(SIGQUIT)
     m_signals.add(SIGQUIT);
   #endif // defined(SIGQUIT)
     // not sure if this is safe here
     m_signals.async_wait(boost::bind(&server::stop, this));

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
    logger_.log("server: connection was removed");
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

void server::handle_stop()
{
    logger_.log("server: stopped listening");
    // The server is stopped by cancelling all outstanding asynchronous
     // operations. Once all operations have finished the io_service::run() call
     // will exit.
     acceptor.close();

     for (auto &connection : connections)
     {
         connections.erase(connection);
         connection->stop();
     }
     connections.clear();
}

}
