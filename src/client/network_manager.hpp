#ifndef NETWORK_MANAGER_HPP
#define NETWORK_MANAGER_HPP

#include "async_logger.hpp"
#include "../common/messages.hpp"
#include "../common/smart_ptr.hpp"
#include "client.hpp"

namespace networking
{

class network_manager
{
public:
    network_manager(smart::fit_smart_ptr<networking::client> client_);
    int get_id_data_from_network();
    void send_position_changed_over_network(int id, int oldx, int oldy,
                                            int posx, int posy);
    void shut_down_client(int player_id);

private:
    smart::fit_smart_ptr<networking::client> client;
};

}



#endif // NETWORK_MANAGER_HPP
