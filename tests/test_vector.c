#include <unity.h>
#include "vectorlib.h"

vector_t vec;

void setUp(void) {
    vector_init(&vec, sizeof(int));
}

void tearDown(void) {
    vector_free(&vec);
}

void test_init(void) {
    TEST_ASSERT_EQUAL(0, vector_size(&vec));
    TEST_ASSERT_GREATER_THAN(0, vector_capacity(&vec));
}

void test_push_and_get(void) {
    int val = 42;
    vector_push_back(&vec, &val);

    int out = 0;
    vector_get(&vec, 0, &out);

    TEST_ASSERT_EQUAL(42, out);
}

void test_pop(void) {
    int val = 10;
    vector_push_back(&vec, &val);

    int out = 0;
    vector_pop_back(&vec, &out);

    TEST_ASSERT_EQUAL(10, out);
    TEST_ASSERT_EQUAL(0, vector_size(&vec));
}

void test_resize_growth(void) {
    for (int i = 0; i < 100; i++) {
        vector_push_back(&vec, &i);
    }

    TEST_ASSERT_EQUAL(100, vector_size(&vec));
}

void test_set(void) {
    int val = 5;
    vector_push_back(&vec, &val);

    int new_val = 99;
    vector_set(&vec, 0, &new_val);

    int out;
    vector_get(&vec, 0, &out);

    TEST_ASSERT_EQUAL(99, out);
}

void test_out_of_bounds(void) {
    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR, vector_get(&vec, 0, &out));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_push_and_get);
    RUN_TEST(test_pop);
    RUN_TEST(test_resize_growth);
    RUN_TEST(test_set);
    RUN_TEST(test_out_of_bounds);

    return UNITY_END();
}
