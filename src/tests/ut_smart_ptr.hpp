#ifndef UT_SMART_PTR_H
#define UT_SMART_PTR_H

namespace smart
{

class ut_smart_ptr
{
public:
    ut_smart_ptr() = default;
    void run_all();

private:
    void test_case_constructor_destructor();
    void test_case_dereference();
    void test_case_copy_and_assignment();
    void test_case_get();
    void test_case_use_count();
    void test_case_comparisions();
    void rvalue_references_reminder();
    void test_case_move_semantics();

    void test_case_polymorphism();
    void dummy_test_case_polymorphism();
};

}

#endif // UT_SMART_PTR_H
