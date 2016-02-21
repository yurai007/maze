#ifndef GUI_DRIVER_HPP
#define GUI_DRIVER_HPP

#include <memory>
#include "abstract_driver.hpp"
#include "client_world_manager.hpp"
#include "renderer.hpp"
#include "client.hpp"
#include "network_manager.hpp"

class gui_driver : public abstract_driver
{
public:
    gui_driver(int argc, char** argv);
    int run(const std::string &ip_address) override;

private:
   int argc_;
   char** argv_;
   Glib::RefPtr<Gtk::Application> application;
};

#endif // GUI_DRIVER_HPP
