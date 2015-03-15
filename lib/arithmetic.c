/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>

const uint128 TEN_18 = 1000000000000000000L;
const uint128 TEN_19 = 10000000000000000000L;
// const uint128 TEN_36 = TEN_19*TEN_19;
const uint128 EIGHT_21 = ((uint128)1)<<(21*3);

unsigned_result_with_carry adc(uint64 x, uint64 y, uint8 carry) {
  unsigned_result_with_carry result;
  uint64 z = x + y + carry;
  int xh = CHECK_HIGHEST_BIT(x);
  int yh = CHECK_HIGHEST_BIT(y);
  int zh = CHECK_HIGHEST_BIT(z);
  int cc = zh ? (xh & yh) : (xh | yh);
  result.value = z;
  result.carry = (uint8) cc;
  return result;
}

unsigned_result_with_carry add(uint64 x, uint64 y) {
  return adc(x, y, 0);
}

unsigned_result_with_carry adc_sx(unsigned_result_with_carry rx, uint64 y) {
  return adc(rx.value, y, rx.carry);
}

unsigned_result_with_carry adc_sy(uint64 x, unsigned_result_with_carry ry) {
  return adc(x, ry.value, ry.carry);
}

unsigned_result_with_carry sbb(uint64 x, uint64 y, uint8 carry) {
  unsigned_result_with_carry result;
  uint64 z = x - y - carry;
  int xh = CHECK_HIGHEST_BIT(x);
  int yh = CHECK_HIGHEST_BIT(y);
  int zh = CHECK_HIGHEST_BIT(z);
  int cc = xh ? (yh & zh) : (yh | zh);
  result.value = z;
  result.carry = (uint8) cc;
  return result;
}

unsigned_result_with_carry sub(uint64 x, uint64 y) {
  return sbb(x, y, 0);
}

uint64 lower(uint128 x) {
  return (uint64) x;
}

uint64 upper(uint128 x) {
  return (uint64) (x >> 64);
}

uint128 combine(uint64 upper, uint64 lower) {
  return (((uint128) upper)<<64) + lower;
}

uint128 mul(uint64 x, uint64 y) {
  uint128 result = (uint128) x * (uint128) y;
  return result;
}

unsigned_divx_result divx(uint128 x, uint64 y) {
  // printf("x=%lld y=%lld\n", (uint64) x, y);
  // fflush(stdout);
  uint128 q = x / y;
  /* printf("q=%lld x=%lld y=%lld\n", (uint64) q, (uint64) x, y); */
  /* fflush(stdout); */
  uint64  r = x % y;
  /* printf("r=%lld q=%lld x=%lld y=%lld\n", r, (uint64) q, (uint64) x, y); */
  /* fflush(stdout); */
  uint64 ql = lower(q);
  /* printf("ql=%lld r=%lld q=%lld x=%lld y=%lld\n", ql, r, (uint64) q, (uint64) x, y); */
  /* fflush(stdout); */
  uint64 qu = upper(q);
  /* printf("qu=%lld ql=%lld r=%lld q=%lld x=%lld y=%lld\n", qu, ql, r, (uint64) q, (uint64) x, y); */
  /* fflush(stdout); */
  unsigned_divx_result result;
  result.quotient = ql;
  result.remainder = r;
  result.overflow = (qu != 0);
  return result;
}

void sprint_uint128_dec(char *buf,  uint128 x) {
  uint64  xl = (uint64) (x % TEN_18);
  uint128 xx = x / TEN_18;
  uint64  xm = (uint64) (xx % TEN_18);
  uint64  xh = (uint64) (xx / TEN_18);
  if (xh != 0) {
    sprintf(buf, "%llu%018llu%018llu", xh, xm, xl);
  } else if (xm != 0) {
    sprintf(buf, "%llu%018llu", xm, xl);
  } else {
    sprintf(buf, "%llu", xl);
  }
}

void sprint_uint128_hex(char *buf,  uint128 x) {
  uint64 xl = lower(x);
  uint64 xh = upper(x);
  if (xh != 0) {
    sprintf(buf, "%llx%016llx", xh, xl);
  } else {
    sprintf(buf, "%llx", xl);
  }
}

void sprint_uint128_oct(char *buf,  uint128 x) {
  uint64  xl = (uint64) (x % EIGHT_21);
  uint128 xx = x / EIGHT_21;
  uint64  xm = (uint64) (xx % EIGHT_21);
  uint64  xh = (uint64) (xx / EIGHT_21);
  if (xh != 0) {
    sprintf(buf, "%llo%021llo%021llo", xh, xm, xl);
  } else if (xm != 0) {
    sprintf(buf, "%llo%021llo", xm, xl);
  } else {
    sprintf(buf, "%llo", xl);
  }
}
