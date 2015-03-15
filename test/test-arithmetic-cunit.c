/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-14
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements tests for psort */

/* enable qsort_r */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <CUnit/Basic.h>

#include <arithmetic.h>

const uint64 uzero = 0;
const uint64 umax_sint64 = ((uint64)1<<63) - 1;
const uint64 umax_uint64 = (uint64) ((sint64) -1);

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

/* 
 */
void test_add_zero() {
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
      for (uint64 xl = 0; xl <= 5; xl++) {
        uint64 x = xh + xl - 1;
        unsigned_result_with_carry result = add(x, 0);
        CU_ASSERT_EQUAL(x, result.value);
        CU_ASSERT_EQUAL(0, result.carry);
        result = add(0, x);
        CU_ASSERT_EQUAL(x, result.value);
        CU_ASSERT_EQUAL(0, result.carry);
        result = adc(x, 0, 0);
        CU_ASSERT_EQUAL(x, result.value);
        CU_ASSERT_EQUAL(0, result.carry);
        result = adc(0, x, 0);
        CU_ASSERT_EQUAL(x, result.value);
        CU_ASSERT_EQUAL(0, result.carry);
      }
  }
}

/* 
 */
void test_sub_zero() {
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 xl = 0; xl <= 5; xl++) {
      uint64 x = xh + xl - 1;
      unsigned_result_with_carry result = sub(x, 0);
      CU_ASSERT_EQUAL(x, result.value);
      CU_ASSERT_EQUAL(0, result.carry);
      result = sbb(x, 0, 0);
      CU_ASSERT_EQUAL(x, result.value);
      CU_ASSERT_EQUAL(0, result.carry);
    }
  }
}

/* 
 */
void test_add_and_sub() {
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 xl = 0; xl <= 5; xl++) {
      uint64 x = xh + xl - 1;
      for (uint64 yh = 1; yh > 0; yh <<= 1) {
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          unsigned_result_with_carry aresult = add(x, y);
          unsigned_result_with_carry lresult = sub(aresult.value, y);
          CU_ASSERT_EQUAL(x, lresult.value);
          unsigned_result_with_carry hresult = sbb(aresult.carry, 0, lresult.carry);
          CU_ASSERT_EQUAL(0, hresult.carry);
          CU_ASSERT_EQUAL(0, hresult.value);
        }
      }
    }
  }
}


/* 
 */
void test_sub_and_add() {
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 xl = 0; xl <= 5; xl++) {
      uint64 x = xh + xl - 1;
      for (uint64 yh = 1; yh > 0; yh <<= 1) {
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          unsigned_result_with_carry lsresult = sub(x, y);
          unsigned_result_with_carry hsresult = sbb(1, 0, lsresult.carry);
          CU_ASSERT(hsresult.value <= 1);
          CU_ASSERT_EQUAL(0, hsresult.carry);
          unsigned_result_with_carry laresult = add(lsresult.value, y);
          unsigned_result_with_carry haresult = adc(hsresult.value, 0, laresult.carry);
          CU_ASSERT_EQUAL(x, laresult.value);
          CU_ASSERT_EQUAL(0, haresult.carry);
          CU_ASSERT_EQUAL(1, haresult.value);
        }
      }
    }
  }
}


/* 
 */
void test_mul_and_div() {
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 xl = 0; xl <= 5; xl++) {
      uint64 x = xh + xl - 1;
      for (uint64 yh = 1; yh > 0; yh <<= 1) {
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          uint128 z = mul(x, y);
          if (y == 0) {
            CU_ASSERT(z == 0);
          } else {
            unsigned_divx_result dx = divx(z, y);
            CU_ASSERT_EQUAL(x, dx.quotient);
            CU_ASSERT_EQUAL(0, dx.remainder);
            CU_ASSERT_EQUAL(0, dx.overflow);
          }
        }
      }
    }
  }
}

/* 
 */
void test_div_and_mul() {
  for (uint128 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      for (uint128 xl = 0; xl <= 5; xl++) {
        uint128 x = xh + xl - 1;
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          if (y == 0) {
            continue;
          }
          uint128 x1 = upper(x);
          unsigned_divx_result z1 = divx(x1, y);
          uint128 x2 = combine(z1.remainder, lower(x));
          unsigned_divx_result z2 = divx(x2, y);
          uint128 q = combine(z1.quotient, z2.quotient);
          uint64  r = z2.remainder;

          CU_ASSERT_EQUAL(0, z1.overflow);
          CU_ASSERT_EQUAL(0, z2.overflow);
          CU_ASSERT(r < y);
          
          uint128 u = mul(lower(q), y);
          uint128 v = mul(upper(q), y);
          uint64 p1 = lower(u);
          unsigned_result_with_carry p2x = add(lower(v), upper(u));
          uint64 p2 = p2x.value;
          unsigned_result_with_carry p3x = adc(upper(v), 0, p2x.carry);
          uint64 p3 = p3x.value;
          CU_ASSERT_EQUAL(0, p3);
          CU_ASSERT_EQUAL(0, p3x.carry);
          unsigned_result_with_carry xx1x = add(p1, r);
          uint64 xx1 = xx1x.value;
          unsigned_result_with_carry xx2x = adc(p2, 0, xx1x.carry);
          uint64 xx2 = xx2x.value;
          uint128 xx = combine(xx2, xx1);
          CU_ASSERT_EQUAL(0, xx2x.carry);
          CU_ASSERT(x == xx);
          CU_ASSERT_EQUAL(x, xx);
        }
      }
    }
  }
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
  if ((NULL == CU_add_test(pSuite, "test of adding 0", test_add_zero))
      || (NULL == CU_add_test(pSuite, "test of subtracting 0", test_sub_zero))
      || (NULL == CU_add_test(pSuite, "test of adding and then subtracting", test_add_and_sub))
      || (NULL == CU_add_test(pSuite, "test of subtracting and then ading", test_sub_and_add))
      || (NULL == CU_add_test(pSuite, "test of multiplication and then division", test_mul_and_div))
      || (NULL == CU_add_test(pSuite, "test of division and then multiplication", test_div_and_mul))
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
