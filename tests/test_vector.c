#include "unity.h"
#include "vectorlib.h"

vector_t vec;

void setUp(void) {
    vector_init(&vec, sizeof(int));
}

void tearDown(void) {
    vector_free(&vec);
}

/* ===================== BASIC TESTS ===================== */

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

/* ===================== EXTENDED TESTS ===================== */

void test_push_multiple_order(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    for (int i = 0; i < 10; i++) {
        int out;
        vector_get(&vec, i, &out);
        TEST_ASSERT_EQUAL(i, out);
    }
}

void test_pop_until_empty(void) {
    int val = 1;
    vector_push_back(&vec, &val);

    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_pop_back(&vec, &out));
    TEST_ASSERT_EQUAL(1, out);

    TEST_ASSERT_EQUAL(VECTOR_RES_ERR, vector_pop_back(&vec, &out));
}

void test_capacity_growth(void) {
    size_t initial = vector_capacity(&vec);

    for (size_t i = 0; i < initial + 1; i++) {
        vector_push_back(&vec, &i);
    }

    TEST_ASSERT_GREATER_THAN(initial, vector_capacity(&vec));
}

void test_set_out_of_bounds(void) {
    int val = 10;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR, vector_set(&vec, 0, &val));
}

void test_get_last_element(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    int out;
    vector_get(&vec, 4, &out);

    TEST_ASSERT_EQUAL(4, out);
}

void test_large_data_integrity(void) {
    for (int i = 0; i < 1000; i++) {
        vector_push_back(&vec, &i);
    }

    for (int i = 0; i < 1000; i++) {
        int out;
        vector_get(&vec, i, &out);
        TEST_ASSERT_EQUAL(i, out);
    }
}

void test_size_consistency(void) {
    int val = 0;

    for (int i = 0; i < 20; i++) {
        vector_push_back(&vec, &val);
    }

    for (int i = 0; i < 10; i++) {
        vector_pop_back(&vec, &val);
    }

    TEST_ASSERT_EQUAL(10, vector_size(&vec));
}

void test_init_zero_size(void) {
    vector_t v;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR, vector_init(&v, 0));
}

void test_free_resets_vector(void) {
    int val = 1;
    vector_push_back(&vec, &val);

    vector_free(&vec);

    TEST_ASSERT_EQUAL(0, vec.size);
    TEST_ASSERT_EQUAL(0, vec.capacity);
    TEST_ASSERT_NULL(vec.data);

    // re-init so tearDown doesn't double free broken state
    vector_init(&vec, sizeof(int));
}

/* ===================== GENERIC TYPE TEST ===================== */

typedef struct {
    int a;
    int b;
} pair_t;

void test_struct_storage(void) {
    vector_free(&vec);
    vector_init(&vec, sizeof(pair_t));

    pair_t p = {1, 2};
    vector_push_back(&vec, &p);

    pair_t out;
    vector_get(&vec, 0, &out);

    TEST_ASSERT_EQUAL(1, out.a);
    TEST_ASSERT_EQUAL(2, out.b);

    // restore int vector for other tests safety
    vector_free(&vec);
    vector_init(&vec, sizeof(int));
}

/* ===================== TEST RUNNER ===================== */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_push_and_get);
    RUN_TEST(test_pop);
    RUN_TEST(test_resize_growth);
    RUN_TEST(test_set);
    RUN_TEST(test_out_of_bounds);

    RUN_TEST(test_push_multiple_order);
    RUN_TEST(test_pop_until_empty);
    RUN_TEST(test_capacity_growth);
    RUN_TEST(test_set_out_of_bounds);
    RUN_TEST(test_get_last_element);
    RUN_TEST(test_large_data_integrity);
    RUN_TEST(test_size_consistency);
    RUN_TEST(test_init_zero_size);
    RUN_TEST(test_free_resets_vector);
    RUN_TEST(test_struct_storage);

    return UNITY_END();
}
