#ifndef SERVER_RESOURCE_HPP
#define SERVER_RESOURCE_HPP

#include <string>
#include <memory>
#include "../common/game_object.hpp"

namespace core
{

class server_resource : public game_object
{
public:
    server_resource(const std::string &name_,
             int posx_, int posy_);
    void tick(unsigned short) override;

private:
    std::string name;
};

}

#endif // SERVER_RESOURCE_HPP
