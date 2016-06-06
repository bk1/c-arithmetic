/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>

// calculate the cbrt of an integer to integer
// this is a proof of concept. To become more useful libraries like gmp should be used.

uint64 cbrt_newton(uint128 x, int rounding_mode);
uint64 cbrt_bit_wise(uint128 x, int rounding_mode);
uint64 cbrt_word_wise(uint128 x, int rounding_mode);
