#ifndef CUSTOM_TRANSPORT_HPP
#define CUSTOM_TRANSPORT_HPP

#include <sys/epoll.h>
#include <signal.h>
#include <functional>

namespace networking
{

/**
 * buffer used to store incoming / outgoing data per connection.
 * It's dynamically allocated chunk of memory in which capacity grows expotentialy.
 * Start is extra information puts by connection.
*/
struct buffer
{
	size_t capacity;
	size_t start, size;
	char *bytes;
};

struct connection_data
{
    int fd;
    uint32_t event;
	buffer data;
};

using t_accept_handler = std::function<void(int error, connection_data *,
                                const char *address, const char *port)>;
using t_read_handler = std::function<void(int bytes_transferred, connection_data *)>;
using t_write_handler = std::function<void(int bytes_transferred, connection_data *)>;
using t_timer_handler = std::function<void(void *context)>;

struct memory_pool;

class reactor final
{
public:
    void init(int port);
    void run();
    void async_accept( t_accept_handler accept_handler );
    void async_read(t_read_handler read_handler, connection_data *connection);
    void async_write(t_write_handler write_handler, connection_data *connection);
    void register_timer(t_timer_handler timer_handler, unsigned ms, void *context);

private:
    t_accept_handler global_accept_handler {nullptr};
    t_read_handler global_read_handler {nullptr};
    t_write_handler global_write_handler {nullptr};
    t_timer_handler global_timer_handler {nullptr};
    void *timer_context {nullptr};
    int server_fd {0}, epoll_fd {0};
    epoll_event *events {nullptr};

    memory_pool *pool {nullptr};
    size_t connections {0};
    static volatile bool interrupted;
    unsigned timeout_ms {0};

    constexpr static unsigned STARTLEN {512};
    constexpr static int MAXEVENTS {128};

    static void check_errors(const char *message, int result);
    static int make_socket_non_blocking(int sfd);
    void reallocate_buffer_exp(buffer *data);
    int allocate_buffer(buffer *data);
    static void free_buffer(buffer *);
    connection_data *allocate_connection(int client_fd);
    static void free_connection(connection_data *connection);
    static void modify_epoll_context(int epoll_fd, int operation, int client_fd,
                                     uint32_t events, void *data);
    static int resolve_name_and_bind(int port);
    void handle_reading_data_from_event(connection_data *connection);
    bool handle_writing_data_to_event(connection_data *connection);
    static void handle_closing(connection_data *connection);
    static void handle_server_closing(int server_fd);
    void handle_accepting_connection(int server_fd);
    void new_handle_accepting_connection(int server_fd, struct epoll_event &client_event,
                                                struct epoll_event &server_event);
    static void interrupt_handler(int , siginfo_t *, void *);
};

}


#endif // CUSTOM_TRANSPORT_HPP
