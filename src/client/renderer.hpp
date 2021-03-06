#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <unordered_map>
#include <tuple>

#include <gtkmm/application.h>
#include <gtkmm/window.h>

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <glibmm/main.h>

#include "client_world_manager.hpp"

namespace core
{
    class server_world_manager;
}

namespace control
{
    class controller;
}

namespace presentation
{

enum class clockwise_rotation
{
    d90, d180, d270, d360
};

class renderer : public Gtk::DrawingArea
{
public:
    renderer(Glib::RefPtr<Gtk::Application> &application);
    void set_world(smart::fit_smart_ptr<core::client_world_manager> world_manager_);
    void draw_circle(int pos_x, int pos_y);
    void draw_image(const std::string &image_name, int pos_x, int pos_y);
    void rotate_image(const std::string &image_name, clockwise_rotation rotation);
    void load_image_and_register(const std::string &image_name, const std::string &path);
    virtual ~renderer() = default;

private:
    void deffered_draw_image(const std::string &image_name, int pos_x, int pos_y);
    bool on_timeout();
    void dummy_circle(const Cairo::RefPtr<Cairo::Context>& cairo_context, int posx, int posy);
    void dummy_text(const Cairo::RefPtr<Cairo::Context>& cairo_context,
                    int pos_y, const std::string &text);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cairo_context) override;

    std::unordered_map<std::string, Glib::RefPtr<Gdk::Pixbuf>> name_to_image;
    std::vector<std::tuple<std::string, int, int>> buffer_calls;

    smart::fit_smart_ptr<core::client_world_manager> world_manager {nullptr};
    Glib::RefPtr<Gtk::Application> app;
};

}

#endif // RENDERER_HPP
