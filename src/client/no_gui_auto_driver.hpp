#ifndef NO_GUI_AUTO_DRIVER_HPP
#define NO_GUI_AUTO_DRIVER_HPP

#include <boost/asio.hpp>
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
    void stop_all();

    io_service m_io_service;
    boost::asio::signal_set m_signals {m_io_service};
    std::unique_ptr<deadline_timer> timer {nullptr};
    std::unique_ptr<boost::posix_time::milliseconds> interval {nullptr};

    int players_number;
    std::vector<smart::fit_smart_ptr<core::client_world_manager>> world_managers;
};

#endif // NO_GUI_AUTO_DRIVER_HPP
