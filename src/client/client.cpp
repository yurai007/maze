#include "client.hpp"
#include <boost/bind.hpp>

namespace networking
{

client::client(const std::string &ip_address)
{
    // TO DO: not sure if this will be working when I will establish real network
    try
    {
        tcp::resolver::query query(ip_address, port, tcp::resolver::query::canonical_name);
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
    catch (...)
    {
        logger_.log("client: exception!");
        assert(false);
    }
}

void client::stop()
{
    io_service.stop();
    logger_.log("client: stopped");
}

}
