#ifndef NO_GUI_DRIVER_HPP
#define NO_GUI_DRIVER_HPP

#include <boost/asio.hpp>
#include <memory>
#include "../common/smart_ptr.hpp"
#include "abstract_driver.hpp"
#include "client_world_manager.hpp"

using namespace boost::asio;

class no_gui_driver : public abstract_driver
{
public:
    no_gui_driver() = default;
    int run(const std::string &ip_address) override;

private:

    void tick(const boost::system::error_code&);

    io_service m_io_service;
    boost::posix_time::milliseconds interval {30};
    deadline_timer timer {m_io_service, interval};
    std::shared_ptr<core::client_world_manager> world_manager {nullptr};
};

#endif // NO_GUI_DRIVER_HPP
