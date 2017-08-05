#include "types.h"
#include "stat.h"
#include "user.h"

int worker(int arg);

int
main() {
    printf(1, "Main : %d\n", worker(3));
    exit();
}

int worker(int arg) {
    return arg+1;
}


