#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include "../common/logger.hpp"
#include "../common/messages.hpp"
#include "client.hpp"

namespace networking
{

class network_manager
{
public:
    network_manager(std::shared_ptr<networking::client> client_);
    messages::get_enemies_data_response get_enemies_data_from_network(int player_id);
    messages::get_players_data_response get_players_data_from_network(int player_id);
    int get_id_data_from_network();
    void send_position_changed_over_network(int id, int oldx, int oldy,
                                            int posx, int posy);
    void shut_down_client(int player_id);

private:
    std::shared_ptr<networking::client> client;
};

}



#endif // NETWORK_MANAGER_HPP