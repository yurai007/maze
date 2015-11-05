#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <memory>
#include <boost/asio.hpp>
#include "connection.h"
#include "../common/logger.hpp"

/*
        *  TCP echo server
        *  No listening (because non-blocking?).
           Under the hood we have connect and accept a'la BSD sockets.
        *  telnet localhost 80. Telnet doesn't work with UDP server.

        1. Ctrl+C send sigal SIGINT to process. Explicit cleaning memory (destructors etc)
           is pointless because OS free all. However it's possible to handle this signal and
           do some work.
        2. connections.assign(max_connections, std::shared_ptr<connection>()) doesnt work.
           Copy assignement operator for shared_ptr is deleted.

        3. Many handlers - all in one thread
        4. It is possible that some connections will be destroyed without knowledge of server
           (delete this)
           How to remove delete this? Delete this in shared_ptr is not good idea :)
           delete this can be replaced by server.remove_connection(shared_from_this())!
           shared_from_this is sth like this but it is safe if this is managed by shared_ptr.In this
           case enable_shared_from_this is required.

        6. boost::bind if takes ref by boost::ref must be carefull.
           Do not pass reference to temporary/local object by boost::ref !!

     * Response time is about 1us.
     * I run 200k clients and it is no problem, I assume 400k is possible as for epoll
*/

namespace networking
{

class message_dispatcher;

class server
{
public:
    server(short port);
    void add_dispatcher(std::shared_ptr<message_dispatcher> dispatcher);
    void run();
    void stop();
    void remove_connection(std::shared_ptr<connection> connection_);
    void send_on_current_connection(const serialization::byte_buffer &data);
    void dispatch_msg_from_buffer(serialization::byte_buffer &buffer);


    std::shared_ptr<connection> current_connection {nullptr};
    io_service m_io_service;

private:
    void register_handler_for_listening();
    void handle_accept(std::shared_ptr<connection> new_connection,
                     const boost::system::error_code& error);
    void handle_stop();


    tcp::acceptor acceptor;
    boost::asio::signal_set m_signals;
    std::shared_ptr<message_dispatcher> m_dispatcher;
    std::set<std::shared_ptr<connection>> connections;
};

}

#endif // SERVER_H
