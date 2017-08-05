#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

void
lock_init(lock_t *m) {
  *m = 0;
}

void
lock_acquire(lock_t *m) {
  while (xchg(m, 1) != 0); 
}

void
lock_release(lock_t *m) {
  *m = 0;
}

void
cond_init(cond_t *cv) {
  *cv = 0;
}

void
cond_wait(cond_t *cv, lock_t *m) {
  cv_wait(cv, m);
}

void
cond_signal(cond_t *cv) {
  cv_signal(cv);
}

void
sem_init(sem_t *s, int value) {
  s->value = value;
  s->cond = 0;
  s->lock = 0;
}

void
sem_wait(sem_t *s) {
  lock_acquire(&s->lock);
  while (s->value <= 0)
    cond_wait(&s->cond, &s->lock);
  s->value--;
  lock_release(&s->lock);
}

void
sem_post(sem_t *s) {
  lock_acquire(&s->lock);
  s->value++;
  cond_signal(&s->cond);
  lock_release(&s->lock);
}
