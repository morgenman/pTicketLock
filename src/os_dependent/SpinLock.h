#ifndef SPINLOCK_H
#define SPINLOCK_H

class SpinLock {
  private:
  //
  // xchg(int *addr, int newval)
  // return what is pointed to by addr
  // at the same time, store newval into addr
  //
  static inline uint xchg(volatile unsigned int *addr, unsigned int newval) {
    uint result;
    asm volatile("lock; xchgl %0, %1" :
                 "+m" (*addr), "=a" (result) :
                 "1" (newval) : "cc");
    return result;
  }

  unsigned int _lock;
  public:
  SpinLock(): _lock{0} {
    // nothing to do
  }

  void lock() volatile {
    while(xchg(&_lock, 1) == 1)
      ; // spin
  }

  void unlock() volatile {
    _lock = 0;
  }
};

#endif /* SPINLOCK_H */
