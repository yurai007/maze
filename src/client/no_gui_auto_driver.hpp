#ifndef NO_GUI_AUTO_DRIVER_HPP
#define NO_GUI_AUTO_DRIVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include "abstract_driver.hpp"
#include "client_world_manager.hpp"

using namespace boost::asio;

class no_gui_auto_driver : public abstract_driver
{
public:
    no_gui_auto_driver(int players_number_);
    int run(const std::string &ip_address) override;

private:

    void tick(const boost::system::error_code&);

    io_service m_io_service;
    boost::posix_time::milliseconds interval {2};
    deadline_timer timer {m_io_service, interval};

    int players_number;
    std::vector<std::shared_ptr<core::client_world_manager>> world_managers;
};

#endif // NO_GUI_AUTO_DRIVER_HPP
