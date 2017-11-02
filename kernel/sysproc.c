#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_fork(void)
{
  return fork();
}

int 
sys_clone(void)
{
  void (*fcn)(void *), *arg;
  if (argptr(0, (void *)&fcn, sizeof(void(*)(void *))) < 0)
      return -1;
  if (argptr2(1, (void *)&arg, sizeof(void(*))) < 0)
      return -1;
  return clone(fcn, arg);
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_join(void)
{
  return join();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int
sys_cv_init(void)
{
  cond_t *cv;
  argptr(0, (void *)&cv, sizeof(cond_t));
  return cv_init(cv);
}

int
sys_cv_wait(void)
{
  cond_t *cv;
  void *lk;
  argptr(0, (void *)&cv, sizeof(cond_t));
  argptr(1, (void *)&lk, sizeof(void *));
  return cv_wait(cv, lk);  
}

int
sys_cv_signal(void)
{
  cond_t *cv;
  argptr(0, (void *)&cv, sizeof(cond_t));
  return cv_signal(cv);
}


// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
