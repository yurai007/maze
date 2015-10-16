#include "client.hpp"
#include <boost/bind.hpp>

namespace networking
{

client::client(const std::string &ip_address)
{
    // TO DO: signals doesn't work here. Maybe I need asynchronous client?
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
  #if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
  #endif
    m_signals.async_wait(boost::bind(&client::stop, this));

    tcp::resolver::query query(tcp::tcp::v4(), ip_address, "5555");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;

    while (error && endpoint_iterator != end)
    {
        socket.close();
        socket.connect(*endpoint_iterator++, error);
    }
    assert(!error);
}

void client::stop()
{
    io_service.stop();
    logger_.log("client: stopped");
}

}
