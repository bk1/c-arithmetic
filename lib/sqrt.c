/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>
#include <sqrt.h>

// calculate the sqrt of an integer to integer
// this is a proof of concept. To become more useful libraries like gmp should be used.

static const uint32 MAX_UINT32             = (uint32) -1; // 0xffffffffffffffff;
static const uint32 MAX_UINT32_M1          = (uint32) -2; // 0xffffffffffffffff;
static const uint32 MAX_UINT32_POW2        = ((uint32) 1) << 31; // 0x8000000000000000;
static const uint32 MAX_UINT32_2BIT_GROUP  = ((uint32) 3) << 30;

static const uint64 MAX_UINT64             = (uint64) -1; // 0xffffffffffffffff;
static const uint64 MAX_UINT64_M1          = (uint64) -2; // 0xffffffffffffffff;
static const uint64 MAX_UINT64_POW2        = ((uint64) 1) << 63; // 0x8000000000000000;
static const uint64 MAX_UINT64_2BIT_GROUP  = ((uint64) 3) << 62;

static const uint128 MAX_UINT128            = (uint128) -1; // 0xffffffffffffffffffffffffffffffff;
static const uint128 MAX_UINT128_M1         = (uint128) -2; // 0xfffffffffffffffffffffffffffffffe;
static const uint128 MAX_UINT128_POW2       = ((uint128) 1) << 127; // 0x80000000000000000000000000000000;
static const uint128 MAX_UINT128_2BIT_GROUP = ((uint128) 3) << 126; // 0xc0000000000000000000000000000000;


void print_u(uint128 u) {
  char bufd[100];
  char bufx[100];
  sprint_uint128_dec(bufd, u);
  sprint_uint128_hex(bufx, u);
  printf("%s (0x%s)", bufd, bufx);

}


/** we are looking for a number y such that y^2 <= x < (y+1)^2 */
uint64 sqrt_newton(uint128 x) {
  // find y, a power of 2, such that y < 2^64 and x/y < 2^64
  // print_u(x);
  // printf("\n");
  // (x == (uint128) 0 || x == (uint128) 1)
  if ((x & MAX_UINT128_M1) == 0) {
    return (uint64) x;
  }
  if (x == MAX_UINT128) {
    return MAX_UINT64;
  }

  uint128 xx = x;
  uint128 yy = x;
  while (xx & MAX_UINT128_M1) {
    xx >>= 2;
    yy >>= 1;
  }
  uint64 y = (uint64) yy;
  uint64 prev_y = y;
  uint64 pprev_y = 0;
  while (1) {
    unsigned_divx_result q = divx(x, y);
    // printf("y=%lld q=%lld r=%lld o=%d\n", y, q.quotient, q.remainder, q.overflow);
    unsigned_result_with_carry sum_low = add(y, q.quotient);
    // printf("sum_low=%llu\n", sum_low.value);
    unsigned_result_with_carry sum_high = adc((uint64) 0, (uint64) q.overflow, sum_low.carry);
    uint64 val_low = sum_low.value;
    val_low >>= 1;
    // printf("val_low=%llu\n", val_low);
    uint64 val_high = sum_high.value;
    if ((val_high & 0x01) == 1) {
      val_low |= MAX_UINT64_POW2;
    }
    // printf("val_low=%llu val_high=%llu\n", val_low, val_high);
    y = val_low;
    // printf("y=%llu\n", y);
    if (y == prev_y) {
      break;
    }
    if (y == pprev_y) {
      if (prev_y < y) {
        y = prev_y;
      }
      return y;
    }

    pprev_y = prev_y;
    prev_y = y;
  }
  return y;
}

uint64 sqrt_bit_wise(uint128 x) {
  // x == 0 || x == 1
  if ((x & MAX_UINT128_M1) == 0) {
    return (uint64) x;
  }
  int shift = 126;
  uint128 pattern = MAX_UINT128_2BIT_GROUP;
  while ((x & pattern) == 0) {
    shift -= 2;
    pattern >>= 2;
  }
  // pattern = 3 * 2**shift
  // x & pattern != 0
  // for all i > shift+1: x & 2^i == 0
  uint64 y = 0;
  uint128 xi = 0;
  while (shift >= 0) {
    // invariant
    // pattern = 3 * 2**shift
    xi = (xi << 2) + ((x & pattern) >> shift);
    uint128 d0 = (((uint128) y) << 2) + 1;
    int      b = 0;
    if (xi >= d0) {
      xi -= d0;
      b = 1;
    }
    y = (y << 1) + b;
    shift -= 2;
    pattern >>= 2;
  }
  return y;
}

unsigned_sqrt_wr_result sqrt_half_bit_wise_with_remainder_internal(uint64 x, int shift, uint64 pattern) {
  // x == 0 || x == 1
  if ((x & MAX_UINT64_M1) == 0) {
    unsigned_sqrt_wr_result result;
    result.sqrt = x;
    result.remainder = 0;
    result.failed = 0;
    return result;
  }
  while ((x & pattern) == 0) {
    shift -= 2;
    pattern >>= 2;
  }
  // pattern = 3 * 2**shift
  // x & pattern != 0
  // for all i > shift+1: x & 2^i == 0
  uint32 y = 0;
  uint64 xi = 0;
  while (shift >= 0) {
    // invariant
    // pattern = 3 * 2**shift
    xi = (xi << 2) + ((x & pattern) >> shift);
    uint64 d0 = (((uint64) y) << 2) + 1;
    int     b = 0;
    if (xi >= d0) {
      xi -= d0;
      b = 1;
    }
    y = (y << 1) + b;
    shift -= 2;
    pattern >>= 2;
  }
  unsigned_sqrt_wr_result result;
  result.sqrt = y;
  result.remainder = xi;
  result.failed = 0;
  return result;
}

unsigned_sqrt_wr_result sqrt_half_bit_wise_with_remainder(uint64 x) {
  int shift = 62;
  uint64 pattern = MAX_UINT64_2BIT_GROUP;
  return sqrt_half_bit_wise_with_remainder_internal(x, shift, pattern);
}

unsigned_sqrt_wr_result sqrt_quarter_bit_wise_with_remainder(uint32 x) {
  int shift = 30;
  uint32 pattern = MAX_UINT32_2BIT_GROUP;
  return sqrt_half_bit_wise_with_remainder_internal((uint64) x, shift, (uint64) pattern);
}

unsigned_sqrt_wr_result calc_sqrt_word_wise_internal(unsigned_sqrt_wr_result step_result, int n0, int n1, int n2, uint64 xlow) {
  uint64 y0 = (uint64) step_result.sqrt;
  uint64 x0 = (uint64) step_result.remainder;

  // printf("y0=%llu (%llx) x0=%llu (%llx) xlow=%llu (%llx) n0=%d n1=%d n2=%d\n", y0, y0, x0, x0, xlow, xlow, n0, n1, n2);

  uint64 x1 = (x0 << n2) + xlow;
  uint64 d0 = ((uint64) y0) << n1;

  // printf("x1=%llu (%llx) d0=%llu (%llx)\n", x1, x1, d0, d0);

  if (d0 == 0) {
    unsigned_sqrt_wr_result result;
    result.failed = 1;
    return result;
  }

  unsigned_divx_result q = divx(x1, d0);
  int was_negative = 0;
  int j = 10;
  uint64 qq = q.quotient;

  // printf("qq=%llu (%llx) q.r=%llu (%llx) q.o=%d\n", qq, qq, q.remainder, q.remainder, q.overflow);

  sint64 r;

  while (1) {
    uint64 d = d0 + qq;
    uint128 qd = mul(qq, d);
    r = (sint64) ((sint64) x1 - (sint128) qd);

    // printf("qq=%llu (%llx) d0=%llu (%llu) d=%llu (%llx) qd=%llu (%llx) r=%lld (%llx)\n", qq, qq, d0, d0, d, d, (uint64) qd, (uint64) qd, r, r);

    if (r >= 0 && (r < d || was_negative)) {
      break;
    }
    if (r < 0) {
      was_negative = 1; // true
      qq--;
    } else {
      qq++;
    }
    if (--j <= 0) {
      // printf("breaking xlow=%llu (%llx) n0=%d n1=%d n2=%d y0=%llu (%llx) x0=%llu (%llx) x1=%llu (%llx) d0=%llu (%llx) qq=%llu (%llx) q0=%llu (%llx)\n", xlow, xlow, n0, n1, n2, y0, y0, x0, x0, x1, x1, d0, d0,qq, qq, q0, q0);
      unsigned_sqrt_wr_result result;
      result.failed = 1;
      return result;
      // break;
    }
  }
  uint64 y = (y0 << n0) + qq;
  unsigned_sqrt_wr_result result;
  result.failed = (j <= 0);
  result.sqrt = y;
  result.remainder = (uint64) r;
  return result;
}

unsigned_sqrt_wr_result sqrt_half_word_wise_with_remainder(uint64 x) {
  uint32 xlow = (uint32) x;
  uint32 xhigh = x >> 32;
  // printf("xlow=%llu (%llx) xhigh=%llu (%llx)\n", (uint64) xlow, (uint64) xlow, (uint64) xhigh, (uint64) xhigh);
  int n0 = 16;
  int n1 = 17;
  int n2 = 32;
  if (xhigh == 0) {
    return sqrt_quarter_bit_wise_with_remainder(xlow);
  } else {
    unsigned_sqrt_wr_result step_result = sqrt_quarter_bit_wise_with_remainder(xhigh);
    unsigned_sqrt_wr_result word_result = calc_sqrt_word_wise_internal(step_result, n0, n1, n2, xlow);
    if (word_result.failed) {
      return sqrt_half_bit_wise_with_remainder(x);
    } else {
      return word_result;
    }
  }
}

uint64 sqrt_word_wise_ori(uint128 x) {
  uint64 xlow = (uint64) x;
  uint64 xhigh = x >> 64;
  int n0 = 32;
  int n1 = 33;
  int n2 = 64;
  unsigned_sqrt_wr_result result;
  if (xhigh == 0) {
    result = sqrt_half_word_wise_with_remainder(xlow);
  } else {
    unsigned_sqrt_wr_result step_result = sqrt_half_word_wise_with_remainder(xhigh);
    result = calc_sqrt_word_wise_internal(step_result, n0, n1, n2, xlow);
  }
  return result.sqrt;
}

uint64 sqrt_word_wise(uint128 x) {
  // avoid edge cases, optimized for x=0 or x=1
  if ((x & MAX_UINT128_M1) == 0) {
    return (uint64) x;
  }
  static const int n0 = 4;
  static const int n1 = n0+1;
  static const int n2 = 2*n0;
  static const int nw = 128/n2;
  static const uint64 pattern = (1<<n2) -1;
  uint32 words[nw];
  size_t count = 0;
  uint128 xx = x;
  while (xx != 0) {
    words[count++] = (uint32) xx & pattern;
    xx >>= n2;
  }


  if (count == 1) {
    unsigned_sqrt_wr_result result = sqrt_quarter_bit_wise_with_remainder(words[0]);
    return result.sqrt;
  }
  uint32 x0 = (words[count-1] << n2) + words[count-2];
  unsigned_sqrt_wr_result step_result = sqrt_quarter_bit_wise_with_remainder(x0);
  if (count == 2) {
    return step_result.sqrt;
  }
  uint64 xi = step_result.remainder;
  uint64 yi = step_result.sqrt;
  for (uint32 i = 3; i <= count; i++) {
    xi = (xi << n2) + words[count - i];
    uint64 d0 = yi << n1;
    if (d0 == 0) {
      printf("bad d0\n");
      return sqrt_bit_wise(x);
    }
    unsigned_divx_result qq = divx((uint128) xi, d0);
    uint64 q = qq.quotient;
    uint64 r = qq.remainder;
    if (qq.overflow) {
      printf("overflow: i=%d\n", i);
      return sqrt_bit_wise(x);
    }
    int j = 10;
    int was_negative = 0;
    while (1) {
      uint64 d = d0 + q;
      uint128 qd = mul(q, d);
      sint128 r = (sint128) xi - (sint128) qd;
      if (r >= 0 && (r < d || was_negative)) {
        break;
      }
      if (r < 0) {
        was_negative = 1;
        q--;
      } else {
        q++;
      }
      if (--j <= 0) {
        printf("breaking i=%d j=%d\n", i, j);
        return sqrt_bit_wise(x);
      }
    }
    xi = r;
    yi = (yi << n0) + q;
  }
  return yi;
}

int no_main(int argc, char **argv) {
  printf(" m=");
  print_u(MAX_UINT128);
  printf("\nmm=");
  print_u(MAX_UINT128_M1);
  printf("\nmp=");
  print_u(MAX_UINT128_POW2);
  printf("\nm2=");
  print_u(MAX_UINT128_2BIT_GROUP);
  printf("\n");
  uint128 x = 1;
  int i;
  int s = atoi(argv[1]);
  int f = atoi(argv[2]);
  int d = atoi(argv[3]);
  int t = atoi(argv[4]);
  printf("s=%d f=%d d=%d t=%d\n", s, f, d, t);

  for (i = 0; i < 200; i++) {
    uint64 yn = sqrt_newton(x);
    uint128 zn = mul(yn, yn);
    uint128 zzn = mul((yn+1), (yn+1));
    unsigned_divx_result qn = divx((uint128) 0, (uint64) 1);
    if (yn != 0) {
      qn = divx(x, yn);
    }
    uint64 yb = sqrt_bit_wise(x);
    uint128 zb = mul(yb, yb);
    uint128 zzb = mul((yb+1), (yb+1));
    unsigned_divx_result qb = divx((uint128) 0, (uint64) 1);
    if (yb != 0) {
      qb = divx(x, yb);
    }
    printf("%4d x=", i);
    print_u(x);
    printf(" -> yn=%llu (%llx) zn=", yn, yn);
    print_u(zn);
    printf(" zzn=");
    print_u(zzn);
    printf(" qn=%llu (0x%llx) rn=%llu (0x%llx) on=%d", qn.quotient, qn.quotient, qn.remainder, qn.remainder, qn.overflow);
    printf("\n");
    printf("%4d x=", i);
    print_u(x);
    printf(" -> yb=%llu (%llx) zb=", yb, yb);
    print_u(zb);
    printf(" zzb=");
    print_u(zzb);
    printf(" qb=%llu (0x%llx) rb=%llu (0x%llx) ob=%d", qb.quotient, qb.quotient, qb.remainder, qb.remainder, qb.overflow);
    printf("\n\n");
    x = (f*(x+s))/d + t;
  }
  exit(0);
}

