#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <gtkmm/window.h>
#include "renderer.hpp"

namespace control
{

class controller : public Gtk::Window
{
public:
    controller() = default;
    virtual ~controller() = default;
    char get_direction() const;
    void reset_direction();

private:
    bool on_key_press_event(GdkEventKey* event) override;
    bool on_key_release_event(GdkEventKey*) override;

    char direction {0};
};

}

#endif // CONTROLLER_HPP
