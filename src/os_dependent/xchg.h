#ifndef XCHG_H
#define XCHG_H

//
// xchg(int *addr, int newval)
// return what is pointed to by addr
// at the same time, store newval into addr
//
/**
 * xchg exchanges the new value with the value in the target address
 * ATOMICALLY.  This permits xchg to serve as the lowest-level
 * primitive for controlled concurrency.
 *
 * @param lock_var the address of the unsigned int which is to be set and tested
 * @param lock_val the value to store in lock_var
 * @returns the _previous_ value stored in lock_var
 */
static inline unsigned int xchg(volatile unsigned int *lock_var,
                                unsigned int lock_val) {
  unsigned int result;
  asm volatile("lock; xchgl %0, %1" :
               "+m" (*lock_var), "=a" (result) :
               "1" (lock_val) : "cc");
  return result;
}

#endif /* XCHG_H */
