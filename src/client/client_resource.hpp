#ifndef CLIENT_RESOURCE_HPP
#define CLIENT_RESOURCE_HPP

#include <string>
#include <memory>
#include "../common/game_object.hpp"
#include "drawable.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{

class client_resource : public game_object, public drawable
{
public:
    client_resource(const std::string &name_,
             smart::fit_smart_ptr<presentation::renderer> renderer_,
             int posx_, int posy_);
    void tick(unsigned short) override;

    void load_image() override;
    void draw(int active_player_x, int active_player_y) override;

private:
    std::string name;
};

}

#endif // CLIENT_RESOURCE_HPP
