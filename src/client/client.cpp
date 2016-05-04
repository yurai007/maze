#include "client.hpp"

namespace networking
{

client::client(const std::string &ip_address)
{
    try
    {
        tcp::resolver::query query(ip_address, port, tcp::resolver::query::canonical_name);
        auto endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;
        boost::system::error_code error = boost::asio::error::host_not_found;

        while (error && endpoint_iterator != end)
        {
            socket.close();
            socket.connect(*endpoint_iterator++, error);
        }
        if (error)
        {
            logger_.log("client: connection error = %d", error.value());
            assert(false);
        }
    }
    catch (std::exception &exception)
    {
        logger_.log("client: exception occured: %s", exception.what());
        assert(false);
    }
}

void client::stop()
{
    io_service.stop();
    logger_.log("client: stopped");
}

}
