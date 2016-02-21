#include <memory>
#include <cassert>

#include "ut_smart_ptr.hpp"
#include "../common/smart_ptr.hpp"

namespace smart
{

void ut_smart_ptr::test_case_constructor_destructor()
{
    static_assert(sizeof(smart_ptr<int>) == sizeof(void*) + sizeof(void*), "should be equal");
    static_assert(sizeof(smart_ptr<smart_ptr<int>>) == sizeof(void*) + sizeof(void*), "should be equal");

    static_assert(std::is_same< smart_ptr<int>::pointer_type, int* >::value, "should be true");
    static_assert(std::is_same< smart_ptr<const char>::pointer_type, const char* >::value, "should be true");

    static_assert(std::is_same< smart_ptr<int>::counter_type, size_t* >::value, "should be true");
    static_assert(std::is_same< smart_ptr<const char>::counter_type, size_t* >::value, "should be true");

    static_assert(std::is_same< smart_ptr<int>::countee_type, size_t >::value, "should be true");
    static_assert(std::is_same< smart_ptr<const char>::countee_type, size_t >::value, "should be true");

    const smart_ptr<const char> ptr1(new char);
    const smart_ptr<const char> ptr2(nullptr);
    const smart_ptr<const char> ptr3(); // that's function declaration!
    const smart_ptr<const char> ptr4(nullptr);
    const smart_ptr<const char> ptr5;
}

void ut_smart_ptr::test_case_dereference()
{
    const smart_ptr<char> ptr1;
    assert(ptr1 == nullptr);
    assert(ptr1.use_count() == 0);

    const smart_ptr<char> ptr2(new char);
    *ptr2 = 123;
    assert(*ptr2 == 123);
}

void ut_smart_ptr::test_case_copy_and_assignment()
{
    smart_ptr<char> ptr1(new char(45));
    assert(*ptr1 == 45);

    smart_ptr<char> ptr2(nullptr);
    smart_ptr<char> ptr3(nullptr);
    auto ptr4(ptr1);
    assert(*ptr4 == 45);

    ptr2 = ptr4;
    assert(*ptr2 == 45);
    ptr3 = ptr1;
    assert(*ptr3 == 45);
    *ptr1 = 123;
    assert(*ptr2 == 123);
    ptr3 = ptr1;
    assert(*ptr3 == 123);

    // nullptr is r-value so move operator=
    ptr4 = smart_ptr<char>(nullptr);
    ptr4 = nullptr;
    ptr3 = smart_ptr<char>(nullptr);
    ptr1 = ptr2 = ptr3;

    smart_ptr<const char> ptr5(new char(12));
    ptr5 = ptr5 = ptr5;
    assert(*ptr5 == 12);
}

void ut_smart_ptr::test_case_get()
{
    smart_ptr<char> ptr1(new char(45));
    assert(ptr1.get() != nullptr);

    smart_ptr<char> ptr2(nullptr);
    assert(ptr2.get() == nullptr);

    smart_ptr<char> ptr3(nullptr);
    assert(ptr3.get() == nullptr);

    auto ptr4(ptr1);
    assert(ptr4.get() == ptr1.get());

    ptr2 = ptr4;
    assert(ptr2.get() == ptr4.get() && ptr4.get() == ptr1.get());
    ptr3 = ptr1;
    assert(ptr3.get() == ptr1.get());
    *ptr1 = 123;
    assert(ptr3.get() == ptr1.get());

    ptr4 = nullptr;
    assert(ptr4.get() == nullptr);
    ptr3 = smart_ptr<char>(nullptr);
    assert(ptr3.get() == nullptr);

    ptr1 = ptr2 = ptr3;
    assert(ptr1.get() == nullptr);
    assert(ptr2.get() == nullptr);
}

void ut_smart_ptr::test_case_use_count()
{
    smart_ptr<char> ptr1(new char(45));
    assert(ptr1.use_count() == 1);

    smart_ptr<char> ptr2(nullptr);
    smart_ptr<char> ptr3(nullptr);
    auto ptr4(ptr1);
    assert(ptr4.use_count() == 2);
    assert(ptr3.use_count() == 0);
    assert(ptr2.use_count() == 0);
    assert(ptr1.use_count() == 2);

    ptr2 = ptr4;
    assert(ptr4.use_count() == 3);
    assert(ptr3.use_count() == 0);
    assert(ptr2.use_count() == 3);
    assert(ptr1.use_count() == 3);
    ptr3 = ptr1;
    assert(ptr3.use_count() == 4);
    assert(ptr4.use_count() == 4);
    assert(ptr2.use_count() == 4);
    assert(ptr1.use_count() == 4);
    *ptr1 = 123;
    assert(ptr3.use_count() == 4);
    assert(ptr4.use_count() == 4);
    assert(ptr2.use_count() == 4);
    assert(ptr1.use_count() == 4);


    // testing overloading for nullptr
    ptr4 = nullptr;
    assert(ptr4.use_count() == 0);
    assert(ptr3.use_count() == 3);
    assert(ptr2.use_count() == 3);
    assert(ptr1.use_count() == 3);
    // NULL is r-value so move operator=
    ptr3 = smart_ptr<char>(nullptr);
    assert(ptr4.use_count() == 0);
    assert(ptr3.use_count() == 0);
    assert(ptr2.use_count() == 2);
    assert(ptr1.use_count() == 2);
    ptr1 = ptr2 = ptr3;
    assert(ptr4.use_count() == 0);
    assert(ptr3.use_count() == 0);
    assert(ptr2.use_count() == 0);
    assert(ptr1.use_count() == 0);
}

void ut_smart_ptr::test_case_comparisions()
{
    smart_ptr<char> ptr1(new char(45));
    assert(ptr1 != nullptr);

    smart_ptr<char> ptr2(nullptr);
    assert(ptr2 == nullptr);

    smart_ptr<char> ptr3(nullptr);
    assert(ptr3 == nullptr);

    auto ptr4 = ptr1;
    assert(ptr4 == ptr1);

    ptr2 = ptr4;
    assert(ptr2 == ptr4 && ptr4 == ptr1);
    ptr3 = ptr1;
    assert(ptr3 == ptr1);
    *ptr1 = 123;
    assert(ptr3 == ptr1);

    ptr4 = nullptr;
    assert(ptr4 == nullptr);
    ptr3 = smart_ptr<char>(nullptr);
    assert(ptr3 == nullptr);

    assert(nullptr != ptr1);
    ptr1 = ptr2 = ptr3;
    assert(ptr1 == nullptr);
    assert(ptr2 == nullptr);

    assert(nullptr == ptr1);
}

void ut_smart_ptr::rvalue_references_reminder()
{
    // here we have normal, old l-value references known as references
    int foo = 1234;
    int &bar = foo;
    assert(bar == 1234);
    // unfortunately in pre-C++11 it was impossible to take reference to
    // r-value (except const&)
    //int &baz = 54321; -- compilation error
    // now it's possible
    int &&baz = 54321; (void)baz;
    // but in this case compilation ofc fails (only r-values):
    // int &&quk = foo;

    // So r-values are typically temporaries
}

void ut_smart_ptr::test_case_move_semantics()
{
    smart_ptr<int> ptr(new int(-123));
    assert(*ptr == -123);
    assert(ptr.use_count() == 1);
    auto ptr2 = std::move(ptr);
    assert(*ptr2 == -123);
    assert(ptr == nullptr);
    assert(ptr.use_count() == 0);
    assert(ptr2.use_count() == 1);
    auto ptr3 = ptr2;
    assert(*ptr3 == -123);
    assert(ptr3.use_count() == 2);
    assert(ptr2.use_count() == 2);
    ptr = ptr3;
    assert(ptr3.use_count() == 3);
    assert(ptr2.use_count() == 3);
    assert(ptr.use_count() == 3);
    ptr = std::move(ptr3);
    assert(ptr3.use_count() == 0);
    assert(ptr3 == nullptr);
    assert(ptr2.use_count() == 2);
    assert(ptr.use_count() == 2);
    auto ptr4 = std::move(ptr2);
    assert(ptr4.use_count() == 2);
    assert(ptr3 == nullptr);
    assert(ptr3.use_count() == 0);
    assert(ptr2 == nullptr);
    assert(ptr2.use_count() == 0);
    assert(ptr.use_count() == 2);
}

class dummy
{
public:
    dummy() = default;
    // trick just against -Wunused-parameter
    dummy(int x) { (void)x; }
    dummy(int x, double y) { (void)x, (void)y; }
    dummy(int x, double y, const dummy &z) { (void)x, (void)y, (void)z; }
};

void ut_smart_ptr::run_all()
{
    test_case_constructor_destructor();
    test_case_dereference();
    test_case_copy_and_assignment();
    test_case_get();
    test_case_use_count();
    test_case_comparisions();
    rvalue_references_reminder();
    test_case_move_semantics();
//    test_case_make_shared();
}

}
