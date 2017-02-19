#include "renderer.hpp"
#include <iostream>
#include <cassert>
#include "async_logger.hpp"

namespace presentation
{

renderer::renderer(Glib::RefPtr<Gtk::Application> &application)
    : app(application)
{
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &renderer::on_timeout), 30 );

    #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
        signal_draw().connect(sigc::mem_fun(*this, &renderer::on_draw), false);
    #endif
}

void renderer::set_world(smart::fit_smart_ptr<core::client_world_manager> world_manager_)
{
    world_manager = world_manager_;
}

void renderer::draw_circle(int pos_x, int pos_y)
{
    // TO DO: just hack
    deffered_draw_image("circle", pos_x, pos_y);
}

void renderer::draw_image(const std::string &image_name, int pos_x, int pos_y)
{
    deffered_draw_image(image_name, pos_x, pos_y);
}

void renderer::rotate_image(const std::string &image_name, clockwise_rotation rotation)
{
    auto &image = name_to_image[image_name];
    auto &image_copy = name_to_image[image_name + "__copy"];
    image_copy->copy_area(0, 0, image->get_width(), image->get_height(), image, 0, 0);

    if (rotation == clockwise_rotation::d360)
        return;

    if (rotation == clockwise_rotation::d90)
         image = image->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE);
    else
        if (rotation == clockwise_rotation::d270)
            image = image->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE);
        else
            image = image->rotate_simple(Gdk::PIXBUF_ROTATE_UPSIDEDOWN);
}

void renderer::load_image_and_register(const std::string &image_name, const std::string &path)
{
    assert(!image_name.empty() && !path.empty());

    if (name_to_image.find(image_name) == name_to_image.end())
    {
        try
        {
            name_to_image[image_name] = Gdk::Pixbuf::create_from_file(path);
            name_to_image[image_name + "__copy"] = Gdk::Pixbuf::create_from_file(path);
        }
        catch(const Gio::ResourceError& exception)
        {
            std::cerr << "Load_image_and_register failed. ResourceError: "
                      << exception.what() << std::endl;
        }
        catch(const Gdk::PixbufError& exception)
        {
            std::cerr << "Load_image_and_register failed. PixbufError: "
                      << exception.what() << std::endl;
        }

        if (name_to_image[image_name])
            set_size_request(name_to_image[image_name]->get_width()/2,
                             name_to_image[image_name]->get_height()/2);
    }
}

void renderer::deffered_draw_image(const std::string &image_name, int pos_x, int pos_y)
{
    buffer_calls.emplace_back(std::make_tuple(image_name, pos_x, pos_y));
}

bool renderer::on_timeout()
{
    Glib::RefPtr<Gdk::Window> window = get_window();
    if (window)
    {
        Gdk::Rectangle rectangle(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        window->invalidate_rect(rectangle, false);
    }
    return true;
}

void renderer::dummy_circle(const Cairo::RefPtr<Cairo::Context>& cairo_context, int posx, int posy)
{
    cairo_context->save();
    cairo_context->translate(posx, posy);
    cairo_context->scale(8, 8);
    cairo_context->arc(0.0, 0.0, 1.0, 0.0, 2 * M_PI);

    cairo_context->set_source_rgba(1.0, 0.0, 0.0, 1.0);
    cairo_context->fill_preserve();
    cairo_context->restore();
    cairo_context->stroke();
}

void renderer::dummy_text(const Cairo::RefPtr<Cairo::Context>& cairo_context, int pos_y,
                          const std::string &text)
{
    // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
    Pango::FontDescription font;

    font.set_family("Monospace");
    font.set_weight(Pango::WEIGHT_BOLD);
    font.set_size(15 * PANGO_SCALE); //

    // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    cairo_context->move_to(0, pos_y);

    layout->show_in_cairo_context(cairo_context);
}

bool renderer::on_draw(const Cairo::RefPtr<Cairo::Context>& cairo_context)
{
    if (world_manager != nullptr)
    {
        // maze have only newest state so moving fields is unecessary
        world_manager->tick_all();
        world_manager->draw_all();

        if (world_manager->killed)
        {
            app->quit();
        }
    }

    for (auto &arguments : buffer_calls)
    {
        const auto &image_name =  std::get<0>(arguments);
        const auto &posx = std::get<1>(arguments);
        const auto &posy = std::get<2>(arguments) - 350;
        if (image_name == "circle")
        {
            dummy_circle(cairo_context, posx, posy);
        }
        else
        {
            assert(name_to_image.find(image_name) != name_to_image.end());
            Gdk::Cairo::set_source_pixbuf(cairo_context, name_to_image[image_name], posx, posy);
            cairo_context->paint();
        }
    }

    std::string player_cash = "Cash: ";
    player_cash += std::to_string(world_manager->player_cash);
    std::string player_health = "Health: ";
    player_health += std::to_string(world_manager->player_health);

    cairo_context->set_source_rgb(1.0, 0.0, 0.0);
    dummy_text(cairo_context, 0,  player_cash);
    dummy_text(cairo_context, 20, player_health);

    buffer_calls.clear();
    return true;
}

}
