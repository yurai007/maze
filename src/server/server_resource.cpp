#include "server_resource.hpp"

namespace core
{

server_resource::server_resource(const std::string &name_,
                   int posx_, int posy_)
    : game_object(posx_, posy_),
      name(name_)
{
}

void server_resource::tick(unsigned short)
{
}

std::string server_resource::get_name() const
{
    return name;
}

}
