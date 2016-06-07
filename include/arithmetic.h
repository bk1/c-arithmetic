/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-14
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */


#ifndef _LIB_ITSKY_ARITHMETIC
#define _LIB_ITSKY_ARITHMETIC

#define CHECK_HIGHEST_BIT(x) ((sint64) (x) < 0)

typedef unsigned int uint32;
typedef signed   int sint32;
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

unsigned_result_with_carry adc(uint64 x, uint64 y, uint8 carry);
unsigned_result_with_carry add(uint64 x, uint64 y);
unsigned_result_with_carry adc_sx(unsigned_result_with_carry rx, uint64 y);
unsigned_result_with_carry adc_sy(uint64 x, unsigned_result_with_carry ry);
unsigned_result_with_carry sbb(uint64 x, uint64 y, uint8 carry);
unsigned_result_with_carry sub(uint64 x, uint64 y);
uint128 mul(uint64 x, uint64 y);
unsigned_divx_result divx(uint128 x, uint64 y);

uint64 lower(uint128 x);
uint64 upper(uint128 x);
uint128 combine(uint64 upper, uint64 lower);

void sprint_uint128_dec(char *buf,  uint128 x);
void sprint_uint128_hex(char *buf,  uint128 x, int with0x);
void sprint_uint128_oct(char *buf,  uint128 x, int with0);
void sprint_sint128_dec(char *buf,  sint128 x);
void sprint_sint128_hex(char *buf,  sint128 x, int with0x);
void sprint_sint128_oct(char *buf,  sint128 x, int with0);

#endif
