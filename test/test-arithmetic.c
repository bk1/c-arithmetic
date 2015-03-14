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
        printf("puts('xh=%llu xl=%llu x=%llu')\n", xh, xl, x);
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
  fprintf(stderr, "done with C-operations\n");
  printf("puts 'DONE'\n");
  fflush(stdout);
  retcode = pclose(pipe);
  handle_error(retcode, "pclose");
  exit(0);
}
