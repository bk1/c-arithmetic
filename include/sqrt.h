/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2016-06-05
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#ifndef _LIB_ITSKY_SQRT
#define _LIB_ITSKY_SQRT

#include <arithmetic.h>

// main API-functions
uint64 sqrt_newton(uint128 x);
uint64 sqrt_bit_wise(uint128 x);
// uint64 sqrt_word_wise(uint128 x);

// helper functions, exposed only for testing TODO move to another header

struct unsigned_sqrt_wr_result {
  uint64 sqrt;
  uint64 remainder;
  int failed;
};

typedef struct unsigned_sqrt_wr_result unsigned_sqrt_wr_result;

unsigned_sqrt_wr_result sqrt_half_bit_wise_with_remainder_internal(uint64 x, int shift, uint64 pattern);
unsigned_sqrt_wr_result sqrt_half_bit_wise_with_remainder(uint64 x); // OK
unsigned_sqrt_wr_result sqrt_quarter_bit_wise_with_remainder(uint32 x); // OK

#endif
