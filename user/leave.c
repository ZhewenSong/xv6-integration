/* clone and play with the argument */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)
int ppid;
int global = 0;
#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();
   int pid[9], i;
   int remains[7] = {0, 1, 2, 3, 4, 6, 7};
   for (i=0;i<9;i++)
     pid[i] = clone(worker, 0);
   sleep(10);
   kill(pid[5]);
   join();
   int npid = clone(worker, 0);
   assert(npid > 0);
   printf(1, "new thread : %d\n", npid);
   sleep(10);
   for (i=0;i<7;i++) {
      kill(pid[remains[i]]);
      join();
   }
   kill(npid);
   join();
   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
    while (1);
}
