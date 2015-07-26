#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include <memory>
#include "game_object.hpp"

namespace presentation
{
    class renderer;
}

namespace core
{

class resource : public game_object
{
public:
    resource(const std::string &name_,
             std::shared_ptr<presentation::renderer> renderer_,
             int posx_, int posy_);
    void load() override;
    void tick() override;
    void draw() override;
    std::tuple<int, int> get_position() const override;

private:
    std::string name;
    std::shared_ptr<presentation::renderer> renderer;
    int posx, posy;
};

}

#endif // RESOURCE_HPP
