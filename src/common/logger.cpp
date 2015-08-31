#include "logger.hpp"
#include <chrono>
#include <cassert>
#include <cstdarg>
#include <thread>

namespace framework
{

logger::logger(bool enabled, bool log_to_file, bool log_date)
    : on(enabled), write_to_file(log_to_file), write_date(log_date)
{
    if (on && write_to_file)
        file_proxy = fopen ("log.txt", "w");
}

// TO DO: lock may be much relaxed probably (maybe even to put_time_in_buffer)
void logger::log(const char *string, ...)
{
    if (!on)
        return;
    char *current_pos = buffer;

    std::lock_guard<std::mutex> lock(mutex);
    if (write_date)
        current_pos = put_time_in_buffer();
    va_list args;
    va_start (args, string);
    vsnprintf (current_pos, max_log_size, string, args);
    va_end (args);

    if (write_to_file)
    {
        fputs(buffer, file_proxy);
        fputs("\n", file_proxy);
    }
    else
        printf("%s\n", buffer);
}


logger::~logger()
{
    if (on && write_to_file)
    {
        fclose (file_proxy);
    }
}


char *logger::put_time_in_buffer()
{
    auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    buffer[0] = '[';
    char *current = &buffer[1];
    if( strftime( current, 20, "%Y-%m-%d %H:%M:%S", localtime( &current_time )) == 0 )
    {
        assert(false);
    }

    buffer[20] = ']';
    buffer[21] = ' ';
    return &buffer[22];
}


}