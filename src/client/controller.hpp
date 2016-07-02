#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <gtkmm/window.h>

namespace control
{

class controller : public Gtk::Window
{
public:
    controller() = default;
    virtual ~controller() = default;
    char get_direction() const;
    bool is_space_on() const;
    void reset();

private:
    bool on_key_press_event(GdkEventKey* event) override;
    bool on_key_release_event(GdkEventKey*) override;

    char direction {0};
    bool is_space {false};
};

}

#endif // CONTROLLER_HPP
