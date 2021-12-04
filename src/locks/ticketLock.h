#ifndef TICKETLOCK_H
#define TICKETLOCK_H
#include <string>
#include <vector>

#include "SpinLock.h"
/**
 * Implementation of the ticket lock type. Note that rather than a
 * queue, the ticket and turn can be represented as integers that
 * count modulo some maximum number. To be safe, making that number
 * greater than or equal to the number of threads is best.
 */
class TicketLock {
 public:
  /**
   * Return type for call to lock. Includes the ticket value, ticket,
   * and the actual turn value the lock had when the ticket was
   * returned.
   */
  class Ticket {
   public:
    const unsigned int ticket;
    const unsigned int initial_turn;
    Ticket(unsigned int t, unsigned int i)
        : ticket{t}, initial_turn{i} { /*Empty body */
    }
  };

  /**
   * Initialize the lock. Set the number of turns/tickets to
   * turnCount. This way you can change the number of threads in the
   * main program and it will always build a safe ticket lock.
   *
   * @param turnCount maximum number of tickets/turns
   */
  TicketLock(unsigned int turnCount);

  /**
   * Issue the thread a ticket and wait until the turn equals the
   * ticket to return. Function spins (it could yield, I suppose)
   * until there is a match. Note that the ticket counter (field) is a
   * _SHARED_, modified variable; it is concurrently modified by
   * multiple threads.
   *
   * Note: Does NOT return until this thread holds the lock.
   *
   * @returns the ticket value issued and the initial turn when
   * waiting started
   */

  Ticket lock() volatile;

  /**
   * Release the lock, permitting some waiting thread (if there are
   * any) into the critical region.
   */
  void unlock() volatile;

 private:
  /**
   * The increment function for any member fields of this class. Do not use ++x
   * or x++. This function takes a little time (based on the last parameter) to
   * perform the increment, holding the old value of the variable locally. This
   * increases the chances of a race condition.
   *
   * @param toBeIncremented the lvalue that is actually changed by the end of
   * this method
   * @param incrementToAdd amount by which to increment the variable; default: 1
   * @param millisecondsToHold delay between read and update; default: 25
   */
  unsigned int evil_increment(volatile unsigned int &toBeIncremented,
                              int incrementToAdd = 1,
                              unsigned int milliseconds = 25) volatile;

  // Implementation details. May include fields (turn, ticket, ...) and methods.
  // Might make use of spin locks, xchg (since it is in the source code), and,
  // perhaps, methods from the book/slides pertaining to a ticket lock.
  unsigned int newTicketUnique;
  unsigned int threads;
  unsigned int turn;
  SpinLock newTicketLock;
};
#endif /* TICKETLOCK_H */
