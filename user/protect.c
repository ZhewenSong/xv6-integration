/*
// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"
#define ASSERT(stmt, text) if (!stmt) printf(1, text);
#define SZ 10
#define PGSZ 4096

char a;

int
main(int argc, char *argv[])
{
  printf(1, "%s", "** Placeholder program for grading scripts **\n");
  char *buf = malloc(SZ * PGSZ);
  ASSERT(buf, "buf not null\n");
  for (int i = 0; i < SZ * PGSZ; i++) {
    buf[i] = 'a';
  }
  mprotect((void*) (((uint) buf) & ~0xFFF), 1);
  for (int i = 0; i < SZ * PGSZ; i++) {
    a = buf[i];
    // should be able to read
  }
  a++;
  if (fork() == 0) {
    for (int i = 0; i < SZ * PGSZ; i++) {
      buf[i] = 'b';
      // should not be able to write
      ASSERT(0, "unreachable\n");
    }
  }
  munprotect((void *) (((uint) buf) & ~0xFFF), 1);
  for (int i = 0; i < SZ * PGSZ; i++) {
    buf[i] = 'b';
    // should be able to write now
  }
  printf(0, "done\n");
  exit();
}
*/

#include "types.h"
#include "stat.h"
#include "user.h"


int
main() {
    int *p = malloc(4096 * 4);
    p[2200] = 10;
    printf(1, "read before : %d\n", p[2200]);
    printf(1, "returned %d\n", mprotect((void *)p, 4));
    printf(1, "read after : %d\n", p[2200]);
    //p[2200]++;
    //printf(1, "write : %d\n", p[2200]);
    
    if (fork() == 0) {
        printf(1, "in child read : %d\n", p[2200]);
        p[2200]++;
        printf(1, "in child write : %d\n", p[2200]);
    }
    wait();
    exit();
}



