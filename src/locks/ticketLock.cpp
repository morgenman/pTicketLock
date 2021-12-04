#include "ticketLock.h"

#include <chrono>
#include <thread>

#include "SpinLock.h"

TicketLock::TicketLock(unsigned int turnCount) {
  threads = turnCount;
  turn = 0;
  newTicketUnique = 0;
  newTicketLock = {};
}

TicketLock::Ticket TicketLock::lock() volatile {
  newTicketLock.lock();
  unsigned int ticket = newTicketUnique;
  newTicketUnique = evil_increment(newTicketUnique);
  newTicketLock.unlock();
  while (ticket != turn)
    ;
  return Ticket{ticket, turn};
}

void TicketLock::unlock() volatile { turn = evil_increment(turn, 1, 0); }

// slowly increment the given variable; increment and delay are both defaulted
unsigned int TicketLock::evil_increment(
    volatile unsigned int &toBeIncremented, int incrementToAdd,
    unsigned int millisecondsToHold) volatile {
  unsigned int initialValue = toBeIncremented;
  std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToHold));
  toBeIncremented = initialValue + incrementToAdd;
  return toBeIncremented;
}
