/* clone and play with the argument */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)
int ppid;
int arg = 1000000;
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
   char *heap = malloc((160 - 19 - 3) * PGSIZE);
   int i;
   for (i=0;i<8;i++)
     assert(clone(worker, 0) > 0);
   for (i=0;i<8;i++)
     assert(join() > 0);
   printf(1, "TEST PASSED\n");
   exit();
}


void
worker(void *arg_ptr) {
    int i;
    for(i=0; i<*(int *)arg_ptr; i++)
        global++;
}
