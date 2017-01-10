#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int
main(int argc, char *argv[])
{
    int num_init = getnumsyscallp();
    int num_final = 0;
    int N = atoi(argv[1]);
    int i;
    for (i=0; i<N; i++)
      num_final += getnumsyscallp();
    printf(1, "%d %d\n", num_init, num_final);
    exit();
}
