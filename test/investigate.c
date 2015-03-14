/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>

#include <arithmetic.h>

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
            printf("cb=%d cc=%d cx=%d cy=%d cz=%d ca=%d\n", cb, cc, cx, cy, cz, carry);
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
            printf("cb=%d cc=%d cx=%d cy=%d cz=%d ca=%d tst=%d\n", cb, cc, cx, cy, cz, carry, tst);
          }
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  short_add_test();
  short_sub_test();
  exit(0);
}
