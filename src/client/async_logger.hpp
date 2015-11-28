#ifndef ASYNC_LOGGER_HPP
#define ASYNC_LOGGER_HPP

#include <cstdio>
#include <memory>
#include <thread>
#include <atomic>

#include "../common/thread_safe_queue.hpp"

namespace framework
{

/* Asynchronous logger. I assume that only one thread use this logger.
   Anyway logger is working on 2 threads internally. Interface thread ('fast thread')
   only puts strings in thread safe queue as producer. I/O thread ('slow thread') consume
   those strings and write to file/stdout.
*/
class async_logger
{
public:

    static async_logger& get()
    {
        static async_logger instance(true, true, true);
        return instance;
    }

    void run();
    void log(const char *string, ...);
    void enable(bool enabled);

private:

    void internal_log(const std::string &buffer);
    //void internal_log_in_place(const char *string, ...);

    async_logger(bool enabled, bool log_to_file, bool log_date);

    template<class Msg>
    void log_message(const Msg &msg)
    {
        log("Message: {name = \"%s\", id = %d}", msg.name.c_str(), msg.id);
    }

    ~async_logger();

    char *put_time_in_buffer();

    bool on;
    const bool write_to_file, write_date;
    bool opened {false};
    static const int max_log_size = 128;
    char buffer[max_log_size];
    FILE * file_proxy = nullptr;

    std::thread io_thread;
    thread_safe_queue<std::string> queue;
    std::atomic<bool> finished {false};
};

}

#define logger_ framework::async_logger::get()

#endif // ASYNC_LOGGER_HPP
