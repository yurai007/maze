#include "sct_server.hpp"
#include "ut_byte_buffer.hpp"
#include "sct_thread_safe_queue.hpp"
#include "sct_async_logger.hpp"
#include "sct_smart_ptr_tests.hpp"
#include "ut_smart_ptr.hpp"
#include "ut_fit_smart_ptr.hpp"
#include "sct_custom_transport.hpp"

#include"../common/message_dispatcher.hpp"
#include"../common/messages.hpp"

namespace networking
{

namespace messages
{

unsigned verify::counter = 0;

void verify_messages()
{
    // not unique
    boost::mpl::for_each<registered_messages>(verify());
}

}
}

static void set_operations_test_case()
{
    std::cout << __FUNCTION__ << std::endl;

    std::vector<int> v1 {1, 2, 5, 5, 5, 9};
    std::vector<int> v2 {2, 5, 7};
    std::vector<int> v1_minus_v2;
    std::vector<int> v2_minus_v1;

    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::inserter(v1_minus_v2, v1_minus_v2.begin()));

    std::set_difference(v2.begin(), v2.end(), v1.begin(), v1.end(),
                        std::inserter(v2_minus_v1, v2_minus_v1.begin()));

    for (auto i : v1_minus_v2)
        std::cout << i << ' ';
    std::cout << '\n';

    for (auto i : v2_minus_v1)
        std::cout << i << ' ';
    std::cout << '\n';


    std::map<int, std::pair<int, int>> m1 {{1, {1, 1}}, {2, {2, 3}}, {5, {5, 5}}, {9, {9, 9}}   };
    std::map<int, std::pair<int, int>> m2 {{2, {2, 2}},  {5, {4, 5}},  {7, {7, 7}} };
    std::map<int, std::pair<int, int>> m1_minus_m2;

    std::set_difference(m1.begin(), m1.end(), m2.begin(), m2.end(),
                        std::inserter(m1_minus_m2, m1_minus_m2.begin()));

    for (auto x : m1_minus_m2)
        std::cout << x.first << "       " << x.second.first << " " << x.second.second << "\n";
    std::cout << '\n';
}


namespace helpers
{

static void concepts_for_dispatcher_test_case()
{
    using fireball = networking::messages::fireball_triggered;
    static_assert(networking::IsMsg<fireball>, "NOK");
    static_assert(!networking::IsMsg<void>, "NOK");

    {
        auto func = [](fireball f){
            return f;
        };
        using Func = decltype(func);
        using my_f_type = decltype(&Func::operator());

        static_assert(networking::IsMsg<networking::function_traits<my_f_type>::arg_type>, "NOK");
        static_assert(networking::IsProperHandler<my_f_type>, "NOK");
    }


    auto dispatcher = smart::smart_make_shared<networking::message_dispatcher>();
    dispatcher->add_handler(
                [&](fireball &msg)
    {
        return msg;
    });

     // OK, but not catched by constraints
//    dispatcher->add_handler(
//                [&](fireball &msg)
//    {
//        (void)msg;
//    });

    // OK, but not catched by constraints
//    dispatcher->add_handler(
//                [&](int msg)
//    {
//        (void)msg;
//    });

    // OK, catched by constraints
//    dispatcher->add_handler(
//                [&]()
//    {

//    });
}


}

int main(int, char*[])
{
//    set_operations_test_case();
    helpers::concepts_for_dispatcher_test_case();

    std::cout << "Running messages verification...\n";
    networking::messages::verify_messages();
    std::cout << "Messages verification passed\n";

    byte_buffer_ut::test_cases();
    thread_safe_queue_sct::test_case();

    smart::ut_smart_ptr plain_tests;
    plain_tests.run_all();
    smart::ut_fit_smart_ptr fit_tests;
    fit_tests.run_all();
    smart_sct::test_case();
    server_sct::test_cases();
    echo_server_component_tests::tests();

    std::cout << "All tests passed\n";
    return 0;
}
