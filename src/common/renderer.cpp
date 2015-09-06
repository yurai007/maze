#include "renderer.hpp"
#include <iostream>
#include <cassert>
#include <memory>

namespace presentation
{

renderer::renderer()
{
    Glib::signal_timeout().connect( sigc::mem_fun(*this, &renderer::on_timeout), 30 );

    #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
        signal_draw().connect(sigc::mem_fun(*this, &renderer::on_draw), false);
    #endif
}

void renderer::set_world(std::shared_ptr<core::client_world_manager> world_manager_)
{
    world_manager = world_manager_;
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

bool renderer::on_draw(const Cairo::RefPtr<Cairo::Context>& cairo_context)
{
    if (world_manager != nullptr)
    {
        world_manager->tick_all();
        world_manager->draw_all();
    }

    for (auto &arguments : buffer_calls)
    {
        const auto &image_name =  std::get<0>(arguments);
        const auto &posx = std::get<1>(arguments);
        const auto &posy = std::get<2>(arguments);
        Gdk::Cairo::set_source_pixbuf(cairo_context, name_to_image[image_name], posx, posy);
        cairo_context->paint();
    }
    buffer_calls.clear();

    return true;
}

}
