/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>

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

uint128 mul(uint64 x, uint64 y) {
  uint128 result = (uint128) x * (uint128) y;
  return result;
}

unsigned_divx_result divx(uint128 x, uint64 y) {
  uint128 q = x / y;
  uint64  r = x % y;
  uint64 ql = lower(q);
  uint64 qu = upper(q);
  unsigned_divx_result result;
  result.quotient = ql;
  result.remainder = r;
  result.overflow = (qu != 0);
  return result;
}
