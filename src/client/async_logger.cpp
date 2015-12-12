#include <chrono>
#include <cassert>
#include <cstdarg>
#include <thread>
#include <ctime>

#include "async_logger.hpp"

namespace framework
{

async_logger::async_logger(bool enabled, bool log_to_file, bool log_date, bool debug_on)
    : on(enabled), write_to_file(log_to_file), write_date(log_date), debug_on_here(debug_on)
{
    if (on && write_to_file)
    {
        file_proxy = fopen ("log.txt", "w");
        opened = true;
    }
}

void async_logger::run()
{
    std::thread local_thread([&]() {

        while (finished == false || !queue.is_empty())
        {
            std::string buffer_dump;
            if (queue.try_pop(&buffer_dump))
                slow_log(buffer_dump);
        }

    });
    io_thread = std::move(local_thread);
}

void async_logger::fast_log(bool in_place, const char *string, ...)
{
    if (!on)
        return;
    char *current_pos = buffer;

    if (write_date)
        current_pos = put_time_in_buffer();
    va_list args;
    va_start (args, string);
    int return_code = vsnprintf (current_pos, max_line_size, string, args);
    assert(return_code >= 0);
    va_end (args);

    std::string buffer_dump(buffer);
    queue.push(buffer_dump);
}

void async_logger::enable(bool enabled)
{
    on = enabled;
}

void async_logger::slow_log(const std::string &buffer)
{
    if (write_to_file)
    {
        fputs(buffer.c_str(), file_proxy);
        fputs("\n", file_proxy); //
        fflush(file_proxy); //??
    }
    else
    {
        printf("%s\n", buffer.c_str());
    }
}

async_logger::~async_logger()
{
    io_thread.join();
    if (opened)
    {
        fclose (file_proxy);
    }
}

char *async_logger::put_time_in_buffer()
{
    auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    int current = 0;
    buffer[current++] = '[';

    size_t written_bytes = strftime(&buffer[current], 20, "%H:%M:%S", localtime( &current_time ));
    assert(written_bytes > 0);
    current += written_bytes;

    struct timespec ts;

    int result = clock_gettime( CLOCK_REALTIME, &ts);
    assert(result == 0);

    int ms = ts.tv_nsec / 1000000L;

    written_bytes = sprintf(&buffer[current], "::%d", ms);
    assert(written_bytes > 0);
    current += written_bytes;

    buffer[current++] = ']';
    buffer[current++] = ' ';
    assert(current < max_line_size);
    return &buffer[current];
}


}
