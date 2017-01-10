#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "connection.hpp"
#include "../common/logger.hpp"
#include "../common/smart_ptr.hpp"
#include "../common/message_dispatcher.hpp"

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


     * From server to generic server (with ReactorPolicy)
     * On the beginning I should align this XYZ server to HTTP (Seastar) server??
       (WHere XYZ = my own protocol a'la HTTP :)
       Need for some generic socket, buffer, error_code, signals, etc.

       Reactor should take:
        - signal handler for signals
        - handlers for events on socket
        - timer handler for periodic action

     * current_connection -> &socket or just port
     * more templates like Protocol + concepts for more readable compilation errors?

     * I need own timer in my reactor (is it possible on epoll with <1ms res?),
       after this I may remove server.get_io_service() in main.cpp and Boost.Asio on server side
       completly

     * Alignment to Seastar HTTP:
       1. Remove sender.send(response); from handlers passing by add_handler
          to dispatcher.
          Handler should somehow return generic msg which will be send only
          in server.
       2. Then we will be allowed to remove ugly send_on_current_connection/
          current_connection.
       3. read_buf, write_buf, generic socket
       4. generic error_type (from Policy) + movement to header
*/

namespace networking
{

constexpr static int sizeof_msg_size = sizeof(unsigned short);

class server
{
public:
    server(short port);
    void add_dispatcher(smart::fit_smart_ptr<message_dispatcher> dispatcher);
    void run();
    void stop();
    void remove_connection(unsigned connection_id);
    io_service &get_io_service();
    smart::fit_smart_ptr<message_dispatcher> m_dispatcher;

private:
    void register_handler_for_listening();
    void handle_accept(smart::fit_smart_ptr<connection> new_connection,
                     const boost::system::error_code& error);
    void handle_stop();

    io_service m_io_service;
    tcp::acceptor acceptor;
    boost::asio::signal_set m_signals;

    std::vector<smart::fit_smart_ptr<connection>> connections;
};

}

#endif // SERVER_H
