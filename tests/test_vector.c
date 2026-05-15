#include "unity.h"
#include "vectorlib.h"

vector_t vec;

void setUp(void) {
    vector_init(&vec, sizeof(int));
}

void tearDown(void) {
    vector_free(&vec);
}

/* ===================== NULL POINTER TESTS ===================== */

void test_init_null_vector(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_init(NULL, sizeof(int)));
}

void test_free_null_vector(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_free(NULL));
}

void test_push_null_vector(void) {
    int val = 1;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_push_back(&null_vec, &val));
}

void test_pop_null_vector(void) {
    int out;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_pop_back(&null_vec, &out));
}

void test_get_null_vector(void) {
    int out;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_get(&null_vec, 0, &out));
}

void test_set_null_vector(void) {
    int val = 1;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_set(&null_vec, 0, &val));
}

void test_clear_null_vector(void) {
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_clear(&null_vec));
}

void test_reserve_null_vector(void) {
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_reserve(&null_vec, 100));
}

void test_shrink_null_vector(void) {
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_shrink(&null_vec, 5));
}

void test_insert_null_vector(void) {
    int val = 1;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_insert(&null_vec, 0, &val));
}

void test_remove_null_vector(void) {
    vector_t null_vec = {0};
    vector_t output = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_remove(&null_vec, 0, 1, &output));
}

void test_capacity_null_vector(void) {
    size_t cap;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_capacity(&null_vec, &cap));
}

void test_size_null_vector(void) {
    size_t sz;
    vector_t null_vec = {0};
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_VECTOR, vector_size(&null_vec, &sz));
}

void test_push_null_input(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_INPUT, vector_push_back(&vec, NULL));
}

void test_get_null_output(void) {
    int val = 1;
    vector_push_back(&vec, &val);
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_OUTPUT, vector_get(&vec, 0, NULL));
}

void test_set_null_input(void) {
    int val = 1;
    vector_push_back(&vec, &val);
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_INPUT, vector_set(&vec, 0, NULL));
}

void test_capacity_null_output(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_OUTPUT, vector_capacity(&vec, NULL));
}

void test_size_null_output(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_OUTPUT, vector_size(&vec, NULL));
}

void test_insert_null_input(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_INPUT, vector_insert(&vec, 0, NULL));
}

/* ===================== INITIALIZATION & SIZE TESTS ===================== */

void test_init_zero_size(void) {
    vector_t v;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_INVALID_SIZE, vector_init(&v, 0));
}

void test_init_valid(void) {
    size_t size, capacity;
    vector_size(&vec, &size);
    vector_capacity(&vec, &capacity);
    TEST_ASSERT_EQUAL(0, size);
    TEST_ASSERT_EQUAL(VECTOR_STD_CAPACITY, capacity);
}

void test_init_various_sizes(void) {
    vector_t v1, v2, v3;

    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_init(&v1, sizeof(char)));
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_init(&v2, sizeof(double)));
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_init(&v3, 256));

    vector_free(&v1);
    vector_free(&v2);
    vector_free(&v3);
}

void test_free_resets_vector(void) {
    int val = 1;
    vector_push_back(&vec, &val);
    vector_free(&vec);

    TEST_ASSERT_EQUAL(0, vec.size);
    TEST_ASSERT_EQUAL(0, vec.capacity);
    TEST_ASSERT_NULL(vec.data);

    // Re-init for tearDown
    vector_init(&vec, sizeof(int));
}

/* ===================== BASIC PUSH/POP/GET/SET TESTS ===================== */

void test_push_and_get(void) {
    int val = 42;
    vector_push_back(&vec, &val);

    int out = 0;
    vector_get(&vec, 0, &out);

    TEST_ASSERT_EQUAL(42, out);
}

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

void test_pop_basic(void) {
    int val = 10;
    vector_push_back(&vec, &val);

    int out = 0;
    vector_pop_back(&vec, &out);

    size_t size;
    vector_size(&vec, &size);

    TEST_ASSERT_EQUAL(10, out);
    TEST_ASSERT_EQUAL(0, size);
}

void test_pop_without_output(void) {
    int val = 10;
    vector_push_back(&vec, &val);

    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_pop_back(&vec, NULL));

    size_t size;
    vector_size(&vec, &size);
    TEST_ASSERT_EQUAL(0, size);
}

void test_pop_empty_vector(void) {
    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_EMPTY, vector_pop_back(&vec, &out));
}

void test_pop_until_empty(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    int out;
    for (int i = 4; i >= 0; i--) {
        TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_pop_back(&vec, &out));
        TEST_ASSERT_EQUAL(i, out);
    }

    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_EMPTY, vector_pop_back(&vec, &out));
}

void test_set_basic(void) {
    int val = 5;
    vector_push_back(&vec, &val);

    int new_val = 99;
    vector_set(&vec, 0, &new_val);

    int out;
    vector_get(&vec, 0, &out);
    TEST_ASSERT_EQUAL(99, out);
}

void test_set_multiple_elements(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    for (int i = 0; i < 10; i++) {
        int new_val = i * 2;
        vector_set(&vec, i, &new_val);
    }

    for (int i = 0; i < 10; i++) {
        int out;
        vector_get(&vec, i, &out);
        TEST_ASSERT_EQUAL(i * 2, out);
    }
}

/* ===================== BOUNDS TESTS ===================== */

void test_get_out_of_bounds(void) {
    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_get(&vec, 0, &out));
}

void test_get_out_of_bounds_after_push(void) {
    int val = 1;
    vector_push_back(&vec, &val);

    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_get(&vec, 1, &out));
}

void test_get_at_size_equals_bounds_error(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    int out;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_get(&vec, 5, &out));
}

void test_get_last_element(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    int out;
    vector_get(&vec, 4, &out);
    TEST_ASSERT_EQUAL(4, out);
}

void test_set_out_of_bounds(void) {
    int val = 10;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_set(&vec, 0, &val));
}

void test_set_out_of_bounds_after_push(void) {
    int val = 1;
    vector_push_back(&vec, &val);

    int new_val = 99;
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_set(&vec, 1, &new_val));
}

/* ===================== CLEAR TESTS ===================== */

void test_clear_keeps_capacity(void) {
    size_t before;
    vector_capacity(&vec, &before);

    for (int i = 0; i < 20; i++) {
        vector_push_back(&vec, &i);
    }

    vector_clear(&vec);

    size_t size, after;
    vector_size(&vec, &size);
    vector_capacity(&vec, &after);

    TEST_ASSERT_EQUAL(0, size);
    TEST_ASSERT_GREATER_OR_EQUAL(after, before);
}

void test_clear_empty_vector(void) {
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_clear(&vec));

    size_t size;
    vector_size(&vec, &size);
    TEST_ASSERT_EQUAL(0, size);
}

void test_clear_multiple_times(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_clear(&vec));
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_clear(&vec));

    size_t size;
    vector_size(&vec, &size);
    TEST_ASSERT_EQUAL(0, size);
}

void test_clear_and_reuse(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    vector_clear(&vec);

    int val = 99;
    TEST_ASSERT_EQUAL(VECTOR_RES_OK, vector_push_back(&vec, &val));

    int out;
    vector_get(&vec, 0, &out);
    TEST_ASSERT_EQUAL(99, out);
}

/* ===================== RESERVE TESTS ===================== */

void test_reserve_growth(void) {
    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_reserve(&vec, 100)
    );

    size_t capacity;
    vector_capacity(&vec, &capacity);

    TEST_ASSERT_EQUAL(100, capacity);
}

void test_reserve_smaller_capacity(void) {
    size_t capacity;
    vector_capacity(&vec, &capacity);

    TEST_ASSERT_EQUAL(
        VECTOR_RES_ERR_INVALID_SIZE,
        vector_reserve(&vec, capacity - 1)
    );
}

void test_shrink_capacity(void) {
    vector_reserve(&vec, 100);

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_shrink(&vec, 50)
    );

    size_t capacity;
    vector_capacity(&vec, &capacity);

    TEST_ASSERT_EQUAL(50, capacity);
}

void test_shrink_clamps_to_std_capacity(void) {
    vector_reserve(&vec, 100);

    vector_shrink(&vec, 1);

    size_t capacity;
    vector_capacity(&vec, &capacity);

    TEST_ASSERT_EQUAL(VECTOR_STD_CAPACITY, capacity);
}

void test_shrink_below_size_fails(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    TEST_ASSERT_EQUAL(
        VECTOR_RES_ERR_INVALID_SIZE,
        vector_shrink(&vec, 5)
    );
}

void test_init(void) {
    size_t size, capacity;

    vector_size(&vec, &size);
    vector_capacity(&vec, &capacity);

    TEST_ASSERT_EQUAL(0, size);
    TEST_ASSERT_GREATER_THAN(0, capacity);
}

void test_pop(void) {
    int val = 10;
    vector_push_back(&vec, &val);

    int out = 0;
    vector_pop_back(&vec, &out);

    size_t size;
    vector_size(&vec, &size);

    TEST_ASSERT_EQUAL(10, out);
    TEST_ASSERT_EQUAL(0, size);
}

void test_resize_growth(void) {
    for (int i = 0; i < 100; i++) {
        vector_push_back(&vec, &i);
    }

    size_t size;

    vector_size(&vec, &size);

    TEST_ASSERT_EQUAL(100, size);
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
    TEST_ASSERT_EQUAL(VECTOR_RES_ERR_BOUNDS, vector_get(&vec, 0, &out));
}

/* ===================== EXTENDED TESTS ===================== */

void test_capacity_growth(void) {
    size_t initial;

    vector_capacity(&vec, &initial);

    for (size_t i = 0; i < initial + 1; i++) {
        vector_push_back(&vec, &i);
    }

    size_t capacity;

    vector_capacity(&vec, &capacity);

    TEST_ASSERT_GREATER_THAN(initial, capacity);
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

    size_t size;
    vector_size(&vec, &size);

    TEST_ASSERT_EQUAL(10, size);
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

/* ===================== INSERT TESTS ===================== */

void test_insert_into_empty(void) {
    int val = 123;

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_insert(&vec, 0, &val)
    );

    int out;
    vector_get(&vec, 0, &out);

    TEST_ASSERT_EQUAL(123, out);
}

void test_insert_front(void) {
    int a = 1, b = 2;

    vector_push_back(&vec, &a);
    vector_insert(&vec, 0, &b);

    int out;

    vector_get(&vec, 0, &out);
    TEST_ASSERT_EQUAL(2, out);

    vector_get(&vec, 1, &out);
    TEST_ASSERT_EQUAL(1, out);
}

void test_insert_middle(void) {
    int a = 1, b = 3, c = 2;

    vector_push_back(&vec, &a);
    vector_push_back(&vec, &b);

    vector_insert(&vec, 1, &c);

    int out;

    vector_get(&vec, 0, &out);
    TEST_ASSERT_EQUAL(1, out);

    vector_get(&vec, 1, &out);
    TEST_ASSERT_EQUAL(2, out);

    vector_get(&vec, 2, &out);
    TEST_ASSERT_EQUAL(3, out);
}

void test_insert_end(void) {
    int a = 1;
    int b = 2;

    vector_push_back(&vec, &a);

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_insert(&vec, 1, &b)
    );

    int out;

    vector_get(&vec, 1, &out);
    TEST_ASSERT_EQUAL(2, out);
}

void test_insert_out_of_bounds(void) {
    int val = 1;

    TEST_ASSERT_EQUAL(
        VECTOR_RES_ERR_BOUNDS,
        vector_insert(&vec, 1, &val)
    );
}

/* ===================== REMOVE TESTS ===================== */

void test_remove_single(void) {
    for (int i = 0; i < 5; i++) {
        vector_push_back(&vec, &i);
    }

    vector_t removed;
    vector_init(&removed, sizeof(int));

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_remove(&vec, 2, 1, &removed)
    );

    int out;

    vector_get(&removed, 0, &out);
    TEST_ASSERT_EQUAL(2, out);

    vector_get(&vec, 2, &out);
    TEST_ASSERT_EQUAL(3, out);

    vector_free(&removed);
}

void test_remove_range(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    vector_t removed;
    vector_init(&removed, sizeof(int));

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_remove(&vec, 3, 4, &removed)
    );

    size_t size;

    vector_size(&vec, &size);
    TEST_ASSERT_EQUAL(6, size);

    int out;

    vector_get(&removed, 0, &out);
    TEST_ASSERT_EQUAL(3, out);

    vector_get(&removed, 3, &out);
    TEST_ASSERT_EQUAL(6, out);

    vector_free(&removed);
}

void test_remove_out_of_bounds(void) {
    vector_t removed;
    vector_init(&removed, sizeof(int));

    TEST_ASSERT_EQUAL(
        VECTOR_RES_ERR_BOUNDS,
        vector_remove(&vec, 0, 1, &removed)
    );

    vector_free(&removed);
}

void test_remove_zero_count(void) {
    vector_t removed;
    vector_init(&removed, sizeof(int));

    TEST_ASSERT_EQUAL(
        VECTOR_RES_ERR_INVALID_SIZE,
        vector_remove(&vec, 0, 0, &removed)
    );

    vector_free(&removed);
}

/* ===================== CAPACITY EDGE TESTS ===================== */

void test_reserve_same_capacity(void) {
    size_t cap;

    vector_capacity(&vec, &cap);

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_reserve(&vec, cap)
    );
}

void test_shrink_exact_size(void) {
    for (int i = 0; i < 10; i++) {
        vector_push_back(&vec, &i);
    }

    TEST_ASSERT_EQUAL(
        VECTOR_RES_OK,
        vector_shrink(&vec, 10)
    );

    size_t cap;

    vector_capacity(&vec, &cap);

    TEST_ASSERT_EQUAL(10, cap);
}

/* ===================== STRESS TESTS ===================== */

void test_many_push_pop_cycles(void) {
    for (int cycle = 0; cycle < 100; cycle++) {

        for (int i = 0; i < 100; i++) {
            vector_push_back(&vec, &i);
        }

        for (int i = 0; i < 100; i++) {
            int out;
            vector_pop_back(&vec, &out);
        }
    }

    size_t size;
    vector_size(&vec, &size);

    TEST_ASSERT_EQUAL(0, size);
}

void test_reallocation_integrity(void) {
    for (int i = 0; i < 5000; i++) {
        vector_push_back(&vec, &i);
    }

    for (int i = 0; i < 5000; i++) {
        int out;
        vector_get(&vec, i, &out);
        TEST_ASSERT_EQUAL(i, out);
    }
}

/* ===================== TEST RUNNER ===================== */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init_null_vector);
    RUN_TEST(test_free_null_vector);
    RUN_TEST(test_push_null_vector);
    RUN_TEST(test_pop_null_vector);
    RUN_TEST(test_get_null_vector);
    RUN_TEST(test_set_null_vector);
    RUN_TEST(test_clear_null_vector);
    RUN_TEST(test_reserve_null_vector);
    RUN_TEST(test_shrink_null_vector);
    RUN_TEST(test_insert_null_vector);
    RUN_TEST(test_remove_null_vector);
    RUN_TEST(test_capacity_null_vector);
    RUN_TEST(test_size_null_vector);

    RUN_TEST(test_push_null_input);
    RUN_TEST(test_get_null_output);
    RUN_TEST(test_set_null_input);
    RUN_TEST(test_capacity_null_output);
    RUN_TEST(test_size_null_output);
    RUN_TEST(test_insert_null_input);

    RUN_TEST(test_init_zero_size);
    RUN_TEST(test_init_valid);
    RUN_TEST(test_init_various_sizes);
    RUN_TEST(test_free_resets_vector);

    RUN_TEST(test_push_and_get);
    RUN_TEST(test_push_multiple_order);
    RUN_TEST(test_pop_basic);
    RUN_TEST(test_pop_without_output);
    RUN_TEST(test_pop_empty_vector);
    RUN_TEST(test_pop_until_empty);

    RUN_TEST(test_set_basic);
    RUN_TEST(test_set_multiple_elements);

    RUN_TEST(test_get_out_of_bounds);
    RUN_TEST(test_get_out_of_bounds_after_push);
    RUN_TEST(test_get_at_size_equals_bounds_error);
    RUN_TEST(test_get_last_element);

    RUN_TEST(test_set_out_of_bounds);
    RUN_TEST(test_set_out_of_bounds_after_push);

    RUN_TEST(test_clear_keeps_capacity);
    RUN_TEST(test_clear_empty_vector);
    RUN_TEST(test_clear_multiple_times);
    RUN_TEST(test_clear_and_reuse);

    RUN_TEST(test_reserve_growth);
    RUN_TEST(test_reserve_smaller_capacity);

    RUN_TEST(test_shrink_capacity);
    RUN_TEST(test_shrink_clamps_to_std_capacity);
    RUN_TEST(test_shrink_below_size_fails);

    RUN_TEST(test_insert_into_empty);
    RUN_TEST(test_insert_front);
    RUN_TEST(test_insert_middle);
    RUN_TEST(test_insert_end);
    RUN_TEST(test_insert_out_of_bounds);

    RUN_TEST(test_remove_single);
    RUN_TEST(test_remove_range);
    RUN_TEST(test_remove_out_of_bounds);
    RUN_TEST(test_remove_zero_count);

    RUN_TEST(test_reserve_same_capacity);
    RUN_TEST(test_shrink_exact_size);

    RUN_TEST(test_capacity_growth);
    RUN_TEST(test_resize_growth);
    RUN_TEST(test_large_data_integrity);
    RUN_TEST(test_size_consistency);

    RUN_TEST(test_many_push_pop_cycles);
    RUN_TEST(test_reallocation_integrity);

    RUN_TEST(test_struct_storage);

    return UNITY_END();
}
