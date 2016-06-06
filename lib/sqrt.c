/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2016-06-05
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>
#include <sqrt.h>

// #define DEBUG_OUTPUT 1

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
    unsigned_result_with_carry sum_low = add(y, q.quotient);
    unsigned_result_with_carry sum_high = adc((uint64) 0, (uint64) q.overflow, sum_low.carry);
    uint64 val_low = sum_low.value;
    val_low >>= 1;
    uint64 val_high = sum_high.value;
    if ((val_high & 0x01) == 1) {
      val_low |= MAX_UINT64_POW2;
    }
    y = val_low;
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
#ifdef DEBUG_OUTPUT
  for (int i = 0; i < count; i++) {
    printf("%2d w=%llu (%02llx)\n", i, (uint64) words[i], (uint64) words[i]);
  }
#endif
  if (count == 1) {
    unsigned_sqrt_wr_result result = sqrt_quarter_bit_wise_with_remainder(words[0]);
    return result.sqrt;
  }
  uint32 x0 = (words[count-1] << n2) + words[count-2];
#ifdef DEBUG_OUTPUT
  printf("x0=%lu (%lx)\n", x0, x0);
#endif
  unsigned_sqrt_wr_result step_result = sqrt_quarter_bit_wise_with_remainder(x0);
  if (count == 2) {
    return step_result.sqrt;
  }
  uint128 xi = step_result.remainder;
  uint64 yi = step_result.sqrt;
#ifdef DEBUG_OUTPUT
  printf("before: xi=");
  print_u(xi);
  printf(" yi=%llu (%llx)\n", yi, yi);
#endif
  for (uint32 i = 3; i <= count; i++) {
#ifdef DEBUG_OUTPUT
      printf("\nouter loop: i=%d\n", i);
#endif
    xi = (xi << n2) + words[count - i];
    uint128 d0 = ((uint128) yi) << n1;
#ifdef DEBUG_OUTPUT
    printf("loop: xi=");
    print_u(xi);
    printf(" d0=");
    print_u(d0);
    printf("\n");
#endif
    if (d0 == 0) {
      printf("bad d0\n");
      return sqrt_bit_wise(x);
    }
    unsigned_divx_result qq;
    qq.overflow = 0;
    if (d0 > xi) {
      qq.quotient = 0;
      qq.remainder = xi;
    } else if (d0 == xi) {
      qq.quotient = 1;
      qq.remainder = 0;
    } else if (d0 > MAX_UINT64) {
      // this case happens, it needs to be addressed in a better way...
#ifdef DEBUG_OUTPUT     
      printf(" d0 too big (1): ");
      printf("d0=");
      print_u(d0);
      printf(" d0\'=%llu (%llx)\n", (uint64) d0, (uint64) d0);
#endif
      return sqrt_bit_wise(x);
    } else if (d0 != (uint64) d0) {
#ifdef DEBUG_OUTPUT     
      printf(" d0 too big (2): ");
      printf("d0=");
      print_u(d0);
      printf(" d0\'=%llu (%llx)\n", (uint64) d0, (uint64) d0);
#endif
      return sqrt_bit_wise(x);
    } else {
      qq = divx((uint128) xi, (uint64) d0);
    }
    if (qq.overflow) {
      printf("overflow: i=%d xi=", i);
      print_u(xi);
      printf(" d0=%llu (%llx) qq=%llu (%llx)\n", (uint64)d0, (uint64) d0, qq.quotient, qq.quotient);
      return sqrt_bit_wise(x);
    }
    uint64 q = qq.quotient;
    uint64 rem = qq.remainder;
    uint64 d0_r = (uint64) d0;
    if (d0 != d0_r) {
#ifdef DEBUG_OUTPUT     
      printf(" d0 too big (3): ");
      printf("d0=");
      print_u(d0);
      printf(" d0_r=%llu (%llx)\n", d0_r, d0_r);
#endif
      return sqrt_bit_wise(x);
    }
#ifdef DEBUG_OUTPUT
    printf("loop: xi=");
    print_u(xi);
    printf(" d0=");
    print_u(d0);
    printf(" d0_r=%llu (%llx)", d0_r, d0_r);
    printf(" q=%llu (%llx) rem=%llu (%llx)\n", q, q, rem, rem);
#endif
    int j = 20;
    int was_negative = 0;
    int was_positive = 0;
    sint128 r = 0;
    sint128 prev_r;
    uint64  prev_q;
    while (1) {
#ifdef DEBUG_OUTPUT
      printf("inner loop: j=%d d0=%llu (%llx) d0=", j, d0_r, d0_r);
      print_u(d0);
      printf("\n");
#endif
      unsigned_result_with_carry urwc_d = add(d0_r, (uint64) q);
      if (urwc_d.carry) {
        printf("overflow calculating d\n");
        return sqrt_bit_wise(x);
      }
      uint64 d = urwc_d.value;
      uint128 qd = mul(q, d);
      r = (sint128) xi - (sint128) qd;
#ifdef DEBUG_OUTPUT
      printf("iloop: j=%d xi=", j);
      print_u(xi);
      printf(" q=%llu (%llx) rem=%llu (%llx) d=%llu (%llx) r=", q, q, rem, rem, d, d);
      print_u(r);
      printf(" sgn=%d\n", r < 0? -1 : 1);
#endif
      if (r == 0 || r >= 0 && (r < d || was_negative)) {
        break;
      }
      if (r < 0) {
        if (was_positive) {
          r = prev_r;
          q = prev_q;
          break;
        }
        was_negative = 1;
        was_positive = 0;
        q--;
      } else {
        was_negative = 0;
        was_positive = 1;
        prev_r = r;
        prev_q = q;
        q++;
      }
#ifdef DEBUG_OUTPUT
      if (j <= 15) {
        printf("pre-breaking i=%d j=%d q=%llu (%llx) xi=", i, j, q, q);
        print_u(xi);
        printf(" d=%llu (%llx) r=", d, d);
        print_u(r);
        printf(" sgn=%d\n", r < 0? -1 : 1);
      }
#endif
      if (--j <= 0) {
        printf("breaking i=%d j=%d q=%llu (%llx) xi=", i, j, q, q);
        print_u(xi);
        printf(" d=%llu (%llx) r=", d, d);
        print_u(r);
        printf(" sgn=%d\n", r < 0? -1 : 1);
        return sqrt_bit_wise(x);
      }
    }
    xi = (uint128) r;
    yi = (yi << n0) + q;
#ifdef DEBUG_OUTPUT
    printf("loop end: xi=");
    print_u(xi);
    printf(" yi=%llu (%llx) q=%llu (%llx)\n", yi, yi, q, q);
#endif
  }
#ifdef DEBUG_OUTPUT
  printf("result: xi=");
  print_u(xi);
  printf(" yi=%llu (%llx)\n", yi, yi);
#endif
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

