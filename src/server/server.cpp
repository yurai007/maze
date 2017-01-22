#include <iostream>
#include <csignal>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "../common/byte_buffer.hpp"
#include "../common/message_dispatcher.hpp"

namespace networking
{

server::server(short port)
      : _listener(port, [this](auto, auto) { this->stop();})
{
    listen();
}

void server::add_dispatcher(smart::fit_smart_ptr<message_dispatcher> dispatcher)
{
    m_dispatcher = dispatcher;
}

void server::run()
{
    _listener.run();
}

void server::stop()
{
    logger_.log("server: stopped listening");
    _listener._io_service.stop();
    _listener.close();

    auto connections_it = connections.begin();
    while (connections_it != connections.end())
    {
        auto connection = *connections_it;
        connection->stop();
        connections_it = connections.erase(connections_it);
    }
    connections.clear();
}

void server::remove_connection(unsigned connection_id)
{
    std::swap(connections.back(), connections[connection_id]);
    connections[connection_id]->id = connection_id;

    auto connection_ = connections.back();
    logger_.log("server: connection with id = %d was removed",
                connection_->get_socket().native_handle());
    connection_->stop();

    connections.pop_back();
}

io_service &server::get_io_service()
{
    return _listener._io_service;
}

void server::listen()
{
    logger_.log("server: start listening");
    tcp::socket boost_socket(_listener._io_service);
    connected_socket socket(std::move(boost_socket));
    auto new_connection = smart::smart_make_shared<connection>(std::move(socket), *this);

    new_connection->id = connections.size();
    connections.push_back(new_connection);

    _listener.listen(
                new_connection->get_socket(),
                [this, new_connection](auto error) mutable {
                        if (!error)
                        {
                            std::string endpoint = boost::lexical_cast<std::string>(
                                                        new_connection->get_socket()._socket.remote_endpoint());
                            logger_.log("server: accepted next connection from %s", endpoint.c_str());
                            new_connection->start();
                        }
                        else
                        {
                            logger_.log("server: connection accepting failed");
                            new_connection = nullptr;
                        }
                        this->listen();
                });
}

}
