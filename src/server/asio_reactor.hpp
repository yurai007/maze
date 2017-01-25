#ifndef ASIO_REACTOR_HPP
#define ASIO_REACTOR_HPP

#include <boost/asio.hpp>

namespace networking
{

using namespace boost::asio;
using ip::tcp;

class asio_connected_socket final
{
public:
    asio_connected_socket(tcp::socket socket)
        : _socket(std::move(socket)) {}

    tcp::socket _socket;

    void close()
    {
        _socket.close();
    }

    int native_handle()
    {
        return _socket.native_handle();
    }

    using error_type = boost::system::error_code;
};

class asio_input_stream final
{
public:
    template<class Func>
    void read_at_least_one_byte(asio_connected_socket &socket, void* dest, size_t bytes,
                                Func func) {
        socket._socket.async_read_some(buffer(dest, bytes), func);
    }
};

class asio_output_stream final
{
public:
    template<class Func>
    void write_all(asio_connected_socket &socket, void* dest, size_t bytes,
                   Func func) {
        async_write(socket._socket, buffer(dest, bytes), func);
    }
};

class asio_reactor final
{
public:
    using connected_socket = asio_connected_socket;
    using input_stream = asio_input_stream;
    using output_stream = asio_output_stream;

    template<class Func>
    asio_reactor(short port, Func func) :
        _io_service(),
        _acceptor(_io_service, tcp::endpoint(tcp::v4(), port)),
        m_signals(_io_service)
    {
        m_signals.add(SIGINT);
        m_signals.add(SIGTERM);
        m_signals.add(SIGQUIT);
        m_signals.async_wait(func);
    }

    template<class Func>
    void listen(asio_connected_socket &socket, Func func) {
        _acceptor.async_accept(socket._socket, func);
    }

    template<class Func>
    void add_timer_handler(unsigned ms, Func timer_handler) {

        interval = std::make_unique<boost::posix_time::milliseconds>(ms);
        timer = std::make_unique<deadline_timer>(_io_service, *interval);
        timer->async_wait([=](auto error_code){ this->update_timer(timer_handler, error_code); });
    }

    template<class Func>
    void update_timer(Func timer_handler, const boost::system::error_code&)
    {
        timer_handler();
        timer->expires_at(timer->expires_at() + *interval);
        timer->async_wait([=](auto error_code){ this->update_timer(timer_handler, error_code); });
    }

    void stop() { _io_service.stop(); }

    void close() { _acceptor.close(); }

    void run()
    {
        _io_service.run();
    }

    connected_socket get_socket()
    {
        tcp::socket boost_socket(_io_service);
        connected_socket socket(std::move(boost_socket));
        return std::move(socket);
    }

    io_service _io_service;
    tcp::acceptor _acceptor;
    boost::asio::signal_set m_signals;
    std::unique_ptr<deadline_timer> timer {nullptr};
    std::unique_ptr<boost::posix_time::milliseconds> interval {nullptr};
};

}

#endif // ASIO_REACTOR_HPP

