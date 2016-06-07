/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2016-06-05
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements tests for c-arithmetic */

/* enable qsort_r */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <CUnit/Basic.h>

#include <math.h>
#include <time.h>

#include <sqrt.h>

static const uint128 MAX_UINT128 = (uint128) -1;
static const uint128 MAX_UINT128_POW2 = ((uint128) 1) << 127;

static const uint64 MAX_UINT64 = (uint64) -1;
static const uint64 MAX_UINT64_POW2 = ((uint64) 1) << 63;

static const uint32 MAX_UINT32 = (uint32) -1;
static const uint32 MAX_UINT32_POW2 = ((uint32) 1) << 31;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1() {
  return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void) {
  return 0;
}

void test_const() {
  printf("\n");
  CU_ASSERT_TRUE(MAX_UINT128_POW2 > 0);
  CU_ASSERT_TRUE(MAX_UINT128 > MAX_UINT128_POW2);
}

void check_with_real_square(uint64 y) {
  uint128 x = mul(y, y);
  uint64 yn = sqrt_newton(x);
  uint64 yb = sqrt_bit_wise(x);
  uint64 yw = sqrt_word_wise(x);
  if (y != yb || y != yw || y!= yn) {
    char bufd[1024];
    char bufx[1024];
    sprint_uint128_dec(bufd, x);
    sprint_uint128_hex(bufx, x, 1);
    printf("          x=%s (%s)\n", bufd, bufx);
    printf(" y=%llu (%llx)\n", y, y);
    printf("yb=%llu (%llx)\n", yb, yb);
    printf("yn=%llu (%llx)\n", yn, yn);
    printf("yw=%llu (%llx)\n", yw, yw);
  }
  CU_ASSERT_EQUAL(y, yb);
  CU_ASSERT_EQUAL(y, yw);
  CU_ASSERT_EQUAL(y, yn);
}

void test_with_real_squares_of_4191() {
  check_with_real_square(4191);
}

/*
 */
void test_with_small_real_squares() {
  printf("\n");
  for (int i = 0; i < 16384; i++) {
    check_with_real_square(i);
  }
}

void test_with_large_real_squares() {
  printf("\n");
  double d = 1.0;
  uint64 y;
  uint64 py = 0;
  while (1) {
    uint128 yy = (uint128) d;
    y = (uint64) yy;
    if (yy == 0 || y < py || yy > MAX_UINT64) {
      break;
    }
    check_with_real_square(y);
    d *= 1.1;
    d += 1.1;
    py = y;
  }
}

/*
 */
void test_near_max_uint128() {
  printf("\n");
  for (int i = 0; i < 16384; i++) {
    uint128 x = MAX_UINT128 - i;
    CU_ASSERT_TRUE(x > MAX_UINT128_POW2);
    uint64 yn = sqrt_newton(x);
    uint64 yb = sqrt_bit_wise(x);
    uint64 yw = sqrt_word_wise(x);
    CU_ASSERT_EQUAL(yb, yn);
    if (yb != yw) {
      char bufd[128], bufx[128];
      sprint_uint128_dec(bufd, x);
      sprint_uint128_hex(bufx, x, 1);
      printf("x=%s (%s) yb=%llu (%llx) yw=%llu (%llx)\n", bufd, bufx, yb, yb, yw, yw);
    }
    CU_ASSERT_EQUAL(yb, yw);
    CU_ASSERT_TRUE(yb <= yw);
    CU_ASSERT_TRUE(mul(yb, yb) <= x);
    unsigned_result_with_carry ybp_low_result = add(yb, 1);
    uint64 ybp_low = ybp_low_result.value;
    uint128 prod = mul(ybp_low, ybp_low);
    CU_ASSERT_TRUE(ybp_low_result.carry > 0 ||  prod > x);
  }
}

void check_sqrt_x(uint128 x, int i) {
  char bufxd[1024];
  char bufxx[1024];
  char bufzd[1024];
  char bufzx[1024];
  char bufzpd[1024];
  char bufzpx[1024];
  uint64 yn = sqrt_newton(x);
  uint64 yb = sqrt_bit_wise(x);
  uint64 yw = sqrt_word_wise(x);
  uint128 zb = mul(yb, yb);
  uint128 zbp = mul(yb+1, yb+1);
  if (zbp <= x) {
    sprint_uint128_dec(bufxd, x);
    sprint_uint128_hex(bufxx, x, 1);
    sprint_uint128_dec(bufzd, zb);
    sprint_uint128_hex(bufzx, zb, 1);
    sprint_uint128_dec(bufzpd, zbp);
    sprint_uint128_hex(bufzpx, zbp, 1);
    printf("i=%7d\n", i);
    printf("          x=%s (%s) yb=%llu (0x%llx) yn=%llu (0x%llx)\n", bufxd, bufxx, yb, yb, yn, yn);
    printf("        y*y=%s (0x%s)\n", bufzd, bufzx);
    printf("(y+1)*(y+1)=%s (0x%s)\n", bufzpd, bufzpx);
  }
  CU_ASSERT_EQUAL(yn, yb);
  if (yb != yw) {
    char bufd[128], bufx[128];
    sprint_uint128_dec(bufd, x);
    sprint_uint128_hex(bufx, x, 1);
    printf("x=%s (%s) yb=%llu (0x%llx) yw=%llu (0x%llx)\n", bufd, bufx, yb, yb, yw, yw);
  }
  CU_ASSERT_TRUE(yb <= yw);
  CU_ASSERT_EQUAL(yb, yw);
  CU_ASSERT_TRUE(zb <= x);
  CU_ASSERT_TRUE(zbp > x);
}

void test_near_powers_of_two() {
  printf("\n");
  uint128 x0 = MAX_UINT128_POW2;
  printf("\n");
  while (x0 != 0) {
    for (int i = 0; i < 1024; i++) {
      uint128 x1 = x0 + i;
      check_sqrt_x(x1, i);
      if (x0 >= i) {
        uint128 x2 = x0 - i;
        check_sqrt_x(x2, -i);
      }
    }
    x0 >>= 1;
  }
}

void test_near_powers_of_three() {
  printf("\n");
  uint128 x0 = 1;
  printf("\n");
  while (x0 < MAX_UINT128_POW2) {
    for (int i = 0; i < 1024; i++) {
      uint128 x1 = x0 + i;
      check_sqrt_x(x1, i);
      if (x0 >= i) {
        uint128 x2 = x0 - i;
        check_sqrt_x(x2, -i);
      }
    }
    x0 *= 3;
  }
}

void test_near_powers_of_pi() {
  printf("\n");
  double d = 1.0;
  printf("\n");
  while (d < (double) MAX_UINT128_POW2) {
    uint128 x0 = (uint128) d;
    for (int i = 0; i < 1024; i++) {
      uint128 x1 = x0 + i;
      check_sqrt_x(x1, i);
      if (x0 >= i) {
        uint128 x2 = x0 - i;
        check_sqrt_x(x2, -i);
      }
    }
    d *= M_PI;
  }
}

void test_performance() {
  printf("\n");
  // test sqrt_bit_wise
  uint128 x;
  uint64 y;
  int n = 10000000;
  clock_t t0, t1;
  t0 = clock();
  for (int i = 0; i < n; i++) {
    x = MAX_UINT128_POW2 - i;
    y = sqrt_bit_wise(x);
  }
  t1 = clock();
  long sqrt_bit_wise_t = (t1 - t0);
  t0 = clock();
  for (int i = 0; i < n; i++) {
    x = MAX_UINT128_POW2 - i;
    y = sqrt_word_wise(x);
  }
  // to make use of y
  CU_ASSERT_TRUE(y >= 0);
  t1 = clock();
  long sqrt_word_wise_t = (t1 - t0);
  t0 = clock();
  for (int i = 0; i < n; i++) {
    x = MAX_UINT128_POW2 - i;
    y = sqrt_newton(x);
  }
  // to make use of y
  CU_ASSERT_TRUE(y >= 0);
  t1 = clock();
  long sqrt_newton_t = (t1 - t0);
  printf("bitwise: %ld wordwise: %ld newton: %ld\n", sqrt_bit_wise_t, sqrt_word_wise_t, sqrt_newton_t);
  // printf("bitwise: %ld newton: %ld\n", sqrt_bit_wise_t, sqrt_newton_t);
}

// helper functions

void check_sqrt_x_half(uint64 x, int i) {
  uint64 yb = sqrt_bit_wise(x);
  unsigned_sqrt_wr_result r = sqrt_half_bit_wise_with_remainder(x);
  uint64 rr = r.remainder;
  uint64 yh = r.sqrt;
  CU_ASSERT_TRUE(rr >= 0);
  CU_ASSERT_EQUAL(x, yh * yh + rr);
  CU_ASSERT_EQUAL(yb, yh);
}

/*
 */
void test_half_bitwise_near_max_uint64() {
  printf("\n");
  for (int i = 0; i < 16384; i++) {
    uint64 x = MAX_UINT64 - i;
    CU_ASSERT_TRUE(x > MAX_UINT64_POW2);
    check_sqrt_x_half(x, i);
  }
}

void check_half_bitwise_near_powers_of_f(int f) {
  printf("\n");
  uint64 x0 = 1;
  while (x0 != 0 && x0 < MAX_UINT64_POW2) {
    for (int i = 0; i < 1024; i++) {
      uint64 x1 = x0 + i;
      check_sqrt_x_half(x1, i);
      if (x0 >= i) {
        uint64 x2 = x0 - i;
        check_sqrt_x_half(x2, -i);
      }
    }
    x0 *= f;
  }
}

void test_half_bitwise_near_powers_of_two() {
  printf("\n");
  check_half_bitwise_near_powers_of_f(2);
}

void test_half_bitwise_near_powers_of_three() {
  printf("\n");
  check_half_bitwise_near_powers_of_f(3);
}

void check_sqrt_x_quarter(uint32 x, int i) {
  uint64 yb = sqrt_bit_wise(x);
  unsigned_sqrt_wr_result r = sqrt_quarter_bit_wise_with_remainder(x);
  uint64 rr = r.remainder;
  uint64 yh = r.sqrt;
  CU_ASSERT_TRUE(rr >= 0);
  CU_ASSERT_EQUAL(x, yh * yh + rr);
  CU_ASSERT_EQUAL(yb, yh);
}

/*
 */
void test_quarter_bitwise_near_max_uint64() {
  printf("\n");
  for (int i = 0; i < 16384; i++) {
    uint32 x = MAX_UINT32 - i;
    CU_ASSERT_TRUE(x > MAX_UINT32_POW2);
    check_sqrt_x_quarter(x, i);
  }
}

void check_quarter_bitwise_near_powers_of_f(int f) {
  printf("\n");
  uint64 x0 = 1;
  while (x0 != 0 && x0 < MAX_UINT32_POW2) {
    for (int i = 0; i < 1024; i++) {
      uint32 x1 = x0 + i;
      check_sqrt_x_quarter(x1, i);
      if (x0 >= i) {
        uint32 x2 = x0 - i;
        check_sqrt_x_quarter(x2, -i);
      }
    }
    x0 *= f;
  }
}

void test_quarter_bitwise_near_powers_of_two() {
  printf("\n");
  check_quarter_bitwise_near_powers_of_f(2);
}

void test_quarter_bitwise_near_powers_of_three() {
  printf("\n");
  check_quarter_bitwise_near_powers_of_f(3);
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main() {

  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) {
    return CU_get_error();
  }

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  if ((NULL == CU_add_test(pSuite, "test near max uint128", test_near_max_uint128))
      || (NULL == CU_add_test(pSuite, "test near powers of 2", test_near_powers_of_two))
      || (NULL == CU_add_test(pSuite, "test near powers of 3", test_near_powers_of_three))
      || (NULL == CU_add_test(pSuite, "test near powers of pi", test_near_powers_of_pi))
      || (NULL == CU_add_test(pSuite, "test with 4191^2", test_with_real_squares_of_4191))
      || (NULL == CU_add_test(pSuite, "test with small real squares", test_with_small_real_squares))
      || (NULL == CU_add_test(pSuite, "test with large real squares", test_with_large_real_squares))
      || (NULL == CU_add_test(pSuite, "test half precision near max uin64", test_half_bitwise_near_max_uint64))
      || (NULL == CU_add_test(pSuite, "test half precision near powers of 2", test_half_bitwise_near_powers_of_two))
      || (NULL == CU_add_test(pSuite, "test half precision near powers of 3", test_half_bitwise_near_powers_of_three))
      || (NULL == CU_add_test(pSuite, "test quarter precision near max uin64", test_quarter_bitwise_near_max_uint64))
      || (NULL == CU_add_test(pSuite, "test quarter precision near powers of 2", test_quarter_bitwise_near_powers_of_two))
      || (NULL == CU_add_test(pSuite, "test quarter precision near powers of 3", test_quarter_bitwise_near_powers_of_three))
      || (NULL == CU_add_test(pSuite, "test performance", test_performance))
      ) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
