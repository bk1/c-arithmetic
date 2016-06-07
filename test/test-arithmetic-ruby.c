/* (C) IT Sky Consulting GmbH 2015
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2015-03-11
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <arithmetic.h>

#define ERROR_SIZE 4096

void add_test() {
  printf("def atest(x,y,ci,z,co)\n    zz=x+y+ci;\n    msg=\"x=#{x} y=#{y} ci=#{ci} z=#{z} co=#{co} zz=#{zz}\"\n    puts(msg+\" zz\") if (zz&0xffffffffffffffff != z);\n    puts(msg+ \" co\") if (zz>>64 != co)\nend\n\n");
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    //printf("xh=%llu\n", xh);
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      //printf("yh=%llu\n", yh);
      for (uint64 xl = 0; xl <= 5; xl++) {
        uint64 x = xh + xl - 1;
        // printf("puts('xh=%llu xl=%llu x=%llu')\n", xh, xl, x);
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

void mul_test() {
  char bufx[128];
  printf("def mtest(x,y,z)\n    zz=x*y;\n    msg=\"x=#{x} y=#{y} z=#{z} zz=#{zz}\"\n    puts(msg) if (z != zz);\nend\n\n");
  for (uint64 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      for (uint64 xl = 0; xl <= 5; xl++) {
        uint64 x = xh + xl - 1;
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          uint128 z = mul(x, y);
          sprint_uint128_hex(bufx, z, 1);
          printf("x=%llu; y=%llu; z=%s;\nmtest(x,y,z);\n", x, y, bufx);
        }
      }
    }
  }
}

void divx_test() {
  char bufx[128];
  char bufq[128];
  printf("def dtest(x,y,q,r,o1,o2)\n    xx=q*y+r;\n    msg=\"x=#{x} y=#{y} q=#{q} r=#{r} xx=#{xx} o1=#{o1} o2=#{o2}\"\n    puts(msg + \" val\") if (x != xx);\n    puts(msg + \" overflow\") if (o1 != 0 || o2 != 0);\nend\n\n");
  fflush(stdout);
  for (uint128 xh = 1; xh > 0; xh <<= 1) {
    for (uint64 yh = 1; yh > 0; yh <<= 1) {
      for (uint128 xl = 0; xl <= 5; xl++) {
        uint128 x = xh + xl - 1;
        for (uint64 yl = 0; yl <= 5; yl++) {
          uint64 y = yh + yl - 1;
          if (y == 0) {
            continue;
          }
          uint128 x1 = upper(x);
          unsigned_divx_result z1 = divx(x1, y);
          uint128 x2 = combine(z1.remainder, lower(x));
          unsigned_divx_result z2 = divx(x2, y);
          uint128 q = combine(z1.quotient, z2.quotient);
          uint64  r = z2.remainder;
          sprint_uint128_hex(bufx, x, 1);
          sprint_uint128_hex(bufq, q, 1);
          printf("x=%s; y=%llu; q=%s; r=%llu; o1=%d; o2=%d;\ndtest(x,y,q,r,o1,o2);\n", bufx, y, bufq, r, z1.overflow, z2.overflow);
          fflush(stdout);
        }
      }
    }
  }
}

void sprintf_test() {
  char bufd[128];
  char bufx[128];
  char bufo[128];
  printf("def ptest(x,y,z, xs, ys, zs)\n    msg=\"x=#{x} (#{xs}) y=#{y} (#{ys}) z=#{z} (#{zs})\"\n    puts(msg+\" x!=y\") if (x != y);\n    puts(msg+ \" x!=z\") if (x != z)\nend\n\n");
  for (uint128 xh = 1; xh > 0; xh <<= 1) {
    for (uint128 xl = 0; xl <= 5; xl++) {
      uint128 x = xh + xl - 1;
      sprint_uint128_dec(bufd, x);
      sprint_uint128_hex(bufx, x, 1);
      sprint_uint128_oct(bufo, x, 1);
      printf("x=%s; xs='%s'; y=%s; ys='%s'; z=%s; zs='%s';\nptest(x,y,z, xs, ys, zs);\n", bufd, bufd, bufx, bufx, bufo, bufo);
    }
  }
}

void handle_error(long code, char *msg) {
  if (code < 0) {
    char extra_msg[ERROR_SIZE];
    int myerrno = errno;
    const char *error_str = strerror(myerrno);
    if (msg != NULL) {
      sprintf(extra_msg, "%s\n", msg);
    } else {
      extra_msg[0] = '\000';
    }
    fprintf(stderr, "%scode=%ld\nerrno=%d\nmessage=%s\n", extra_msg, code, myerrno, error_str);
    exit(1);
  }
}

int main(int argc, char **argv) {
  FILE *pipe = popen("tee arithmetic.log | /usr/bin/ruby", "w");
  if (pipe == NULL) {
    handle_error(-1, "popen");
  }
  int pfd = fileno(pipe);
  handle_error(pfd, "fileno");
  int retcode = close(STDOUT_FILENO);
  handle_error(retcode, "close(STDOUT_FILENO)");
  retcode = dup(pfd);
  handle_error(retcode, "dup");
  fprintf(stderr, "doing add_test\n");
  add_test();
  fprintf(stderr, "doing sub_test\n");
  sub_test();
  fprintf(stderr, "doing sprintf_test\n");
  sprintf_test();
  fprintf(stderr, "doing mul_test\n");
  mul_test();
  fprintf(stderr, "doing divx_test\n");
  divx_test();
  fprintf(stderr, "done with C-operations\n");
  printf("puts 'DONE'\n");
  fflush(stdout);
  retcode = close(STDOUT_FILENO);
  handle_error(retcode, "close");
  retcode = pclose(pipe);
  handle_error(retcode, "pclose");
  exit(0);
}
