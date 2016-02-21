#include "sct_server.hpp"
#include "ut_byte_buffer.hpp"
#include "sct_thread_safe_queue.hpp"
#include "sct_async_logger.hpp"
#include "sct_smart_ptr_tests.hpp"
#include "ut_smart_ptr.hpp"
#include "ut_fit_smart_ptr.hpp"

int main(int, char*[])
{
    byte_buffer_ut::test_cases();
    thread_safe_queue_sct::test_case();

    smart::ut_smart_ptr plain_tests;
    plain_tests.run_all();
    smart::ut_fit_smart_ptr fit_tests;
    fit_tests.run_all();
    smart_sct::test_case();

    //server_sct::test_cases();
    std::cout << "All tests passed\n";
    return 0;
}
