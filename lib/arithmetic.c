/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <itskylib.h>

#define CHECK_HIGHEST_BIT(x) ((sint64) (x) < 0)

typedef unsigned long long uint64;
typedef signed   long long sint64;
typedef __uint128_t uint128;
typedef __int128_t  sint128;
typedef unsigned short uint16;
typedef unsigned char uint8;

struct signed_result_with_carry {
  sint64 value;
  uint8 carry;
};

struct unsigned_result_with_carry {
  uint64 value;
  uint8 carry;
};

struct unsigned_divx_result {
  uint64 quotient;
  uint64 remainder;
  uint8 overflow;
};

typedef struct unsigned_result_with_carry unsigned_result_with_carry;
typedef struct signed_result_with_carry signed_result_with_carry;
typedef struct unsigned_divx_result unsigned_divx_result;

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

void short_add_test() {
  for (uint16 xh = 0; xh <= 1; xh++) {
    for (uint16 yh = 0; yh <= 1; yh++) {
      for (uint16 xl = 0; xl <= 0x7fff; xl++) {
        uint16 x = xh*0x8000 + xl;
        if (xl == 0x0080) {
          xl = 0x7f00;
        }
        for (uint16 yl = 0; yl <= 0x7fff; yl++) {
          uint16 y = yh*0x8000 + yl;
          if (yl == 0x0080) {
            yl = 0x7f00;
          }
          for (uint8 carry = 0; carry <= 1; carry++) {
            int xx = x;
            int yy = y;
            int zz = xx + yy + carry;
            int cb = (0x10000 & zz) != 0;
            uint16 z = x+y+carry;
            int cx = (x & 0x8000) != 0;
            int cy = (y & 0x8000) != 0;
            int cz = (z & 0x8000) != 0;
            int cc = (cx & ! cz) | (cy & ! cz) | (cx & cy & cz);
            // printf("cb=%d cc=%d cx=%d cy=%d cz=%d (x=%d y=%d z=%d xx=%d yy=%d zz=%d)\n", cb, cc, cx, cy, cz, (int) x, (int) y, (int) z, xx, yy, zz);
            printf("cb=%d cc=%d cx=%d cy=%d cz=%d ca=%d\n", cb, cc, cx, cy, cz, carry);
          }
        }
      }
    }
  }
}

void add_test() {
  printf("def atest(x,y,ci,z,co)\n    zz=x+y+ci;\n    msg=\"x=#{x} y=#{y} ci=#{ci} z=#{z} co=#{co} zz=#{zz}\"\n    puts(msg+\" zz\") if (zz&0xffffffffffffffff != z);\n    puts(msg+ \" co\") if (zz>>64 != co)\nend\n\n");
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    //printf("xh=%llu\n", xh);
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      //printf("yh=%llu\n", yh);
      for (uint64 xl = 0; xl <= 5; xl++) {
        uint64 x = xh + xl - 1;
        //printf("xh=%llu xl=%llu x=%llu\n", xh, xl, x);
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          //printf("yh=%llu yl=%llu y=%llu\n", yh, yl, y);
          for (uint8 carry = 0; carry <= 1; carry++) {
            unsigned_result_with_carry z = adc(x, y, carry);
            printf("x=%llu; y=%llu; ci=%d; z=%llu; co=%d;\natest(x,y,ci,z,co);\n", x, y, (int) carry, z.value, (int) z.carry);
          }
        }
      }
    }
  }
}

void sub_test() {
  printf("def stest(x,y,ci,z,co)\n    zz=x-y-ci+(1<<64);\n    msg=\"x=#{x} y=#{y} ci=#{ci} z=#{z} co=#{co} zz=#{zz}\"\n    puts(msg+\" zz\") if (zz&0xffffffffffffffff != z);\n    puts(msg+ \" co\") if (1-(zz>>64) != co)\nend\n\n");
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    //printf("xh=%llu\n", xh);
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      //printf("yh=%llu\n", yh);
      for (uint64 xl = 0; xl <= 5; xl++) {
        uint64 x = xh + xl - 1;
        //printf("xh=%llu xl=%llu x=%llu\n", xh, xl, x);
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          //printf("yh=%llu yl=%llu y=%llu\n", yh, yl, y);
          for (uint8 carry = 0; carry <= 1; carry++) {
            unsigned_result_with_carry z = sbb(x, y, carry);
            printf("x=%llu; y=%llu; ci=%d; z=%llu; co=%d;\nstest(x,y,ci,z,co);\n", x, y, (int) carry, z.value, (int) z.carry);
          }
        }
      }
    }
  }
}

void short_sub_test() {
  for (uint16 xh = 0; xh <= 1; xh++) {
    for (uint16 yh = 0; yh <= 1; yh++) {
      for (uint16 xl = 0; xl <= 0x7fff; xl++) {
        uint16 x = xh*0x8000 + xl;
        if (xl == 0x0080) {
          xl = 0x7f00;
        }
        for (uint16 yl = 0; yl <= 0x7fff; yl++) {
          uint16 y = yh*0x8000 + yl;
          if (yl == 0x0080) {
            yl = 0x7f00;
          }
          for (uint8 carry = 0; carry <= 1; carry++) {
            int xx = x;
            int yy = y;
            int zz = xx - yy - carry;
            int cb = zz < 0;
            uint16 z = x-y-carry;
            int cx = (x & 0x8000) != 0;
            int cy = (y & 0x8000) != 0;
            int cz = (z & 0x8000) != 0;
            int cc = (cy | cz) & ! cx | cx & cy & cz;
            int tst = cb-cc;
            // printf("cb=%d cc=%d cx=%d cy=%d cz=%d (x=%d y=%d z=%d xx=%d yy=%d zz=%d)\n", cb, cc, cx, cy, cz, (int) x, (int) y, (int) z, xx, yy, zz);
            printf("cb=%d cc=%d cx=%d cy=%d cz=%d ca=%d tst=%d\n", cb, cc, cx, cy, cz, carry, tst);
          }
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  sub_test();
  exit(0);
}
