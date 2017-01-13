#include "network_manager.hpp"

namespace networking
{
network_manager::network_manager(smart::fit_smart_ptr<networking::client> client_)
    : client(client_)
{
    assert(client != nullptr);
}

// TO DO: commented because log_debug_in_place does not work yet

messages::get_enemies_data_response network_manager::get_enemies_data_from_network(int player_id)
{
    messages::get_enemies_data request(player_id);
    client->send_request(request);

    auto response = client->read_get_enemies_data_response();

    logger_.log_debug("client_world_manager%d: get_enemies_data was load. Content dump:", player_id);
    size_t i = 0;
    for (; i < response.content.size(); i += 3)
    {
        if (i != 0 && (i % 15 == 0) )
            logger_.log_debug("{%d, %d, %d}", response.content[i], response.content[i+1],
                response.content[i+2]);
        else
            logger_.log_debug("{%d, %d, %d} ", response.content[i], response.content[i+1],
                response.content[i+2]);
    }
    if ((i-3)%15 != 0)
        logger_.log_debug("\n");
    assert(response.content.size() % 3 == 0);
    return response;
}

messages::get_players_data_response network_manager::get_players_data_from_network(int player_id)
{
    messages::get_players_data request;
    client->send_request(request);

    auto response = client->read_get_players_data_response();

    logger_.log_debug("client_world_manager%d: get_players_data was load. Content dump:", player_id);
    size_t i = 0;
    for (; i < response.content.size(); i += 3)
    {
        if (i != 0 && (i % 15 == 0) )
            logger_.log_debug("{%d, %d, %d}\n", response.content[i], response.content[i+1],
                response.content[i+2]);
        else
            logger_.log_debug("{%d, %d, %d} ", response.content[i], response.content[i+1],
                response.content[i+2]);
    }
    if ((i-3)%15 != 0)
        logger_.log_debug("\n");
    assert(response.content.size() % 3 == 0);

    return response;
}

messages::get_resources_data_response network_manager::get_resources_data_from_network()
{
    messages::get_resources_data request;
    client->send_request(request);

    auto response = client->read_get_resources_data_response();

    logger_.log_debug("client_world_manager: get_resources_data was load");

    assert(response.content.size() % 3 == 0);
    return response;
}

int network_manager::get_id_data_from_network()
{
    messages::get_id request;
    client->send_request(request);

    auto response = client->read_get_id_response();

    logger_.log("client_world_manager: player_id = %d", response.player_id);
    assert(response.player_id > 0);
    return response.player_id;
}

void network_manager::send_position_changed_over_network(int id, int oldx, int oldy,
                                                         int posx, int posy)
{
    messages::position_changed request = {id, oldx, oldy, posx, posy};
    client->send_request(request);
    auto response = client->read_position_changed_response();
    assert(response.content == "OK");
    logger_.log("client_player: id = %d, send position_changed request = {%d, %d} -> {%d, %d} "
                "and got response", id, oldx, oldy, posx, posy);
}

void network_manager::shut_down_client(int player_id)
{
    networking::messages::client_shutdown msg = {player_id};
    client->send_request(msg);
}

}


