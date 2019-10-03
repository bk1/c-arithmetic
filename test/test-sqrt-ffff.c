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

// static const uint128 MAX_UINT128 = (uint128) -1;
// static const uint128 MAX_UINT128_POW2 = ((uint128) 1) << 127;

// static const uint64 MAX_UINT64 = (uint64) -1;
// static const uint64 MAX_UINT64_POW2 = ((uint64) 1) << 63;

// static const uint32 MAX_UINT32 = (uint32) -1;
// static const uint32 MAX_UINT32_POW2 = ((uint32) 1) << 31;

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

void test_ffff() {
  printf("\n");
  uint128  x = (((uint128) 1) << 126) + 1;
  uint64  yb = sqrt_bit_wise(x);
  uint64  yw = sqrt_word_wise(x);
  uint64  y_expected = ((uint64) 1) << 63;

  char bufd[1024];
  char bufx[1024];
  sprint_uint128_dec(bufd, x);
  sprint_uint128_hex(bufx, x, 1);
  printf("  0x40000000000000000000000000000001\n");
  printf("          x=%s (%s)\n", bufd, bufx);
  printf("yb=%llu (0x%llx)\n", yb, yb);
  printf("yw=%llu (0x%llx)\n", yw, yw);

  CU_ASSERT_EQUAL(y_expected, yb);
  CU_ASSERT_EQUAL(y_expected, yw);
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
  if ((NULL == CU_add_test(pSuite, "test near max uint128", test_ffff))
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
