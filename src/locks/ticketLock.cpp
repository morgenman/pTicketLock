#include "ticketLock.h"

#include <chrono>
#include <thread>

#include "SpinLock.h"

TicketLock::TicketLock(unsigned int turnCount) {
  maxThreads = turnCount;
  for (unsigned int i = 0; i < turnCount; i++) threads.push_back(i);
  newTicketUnique = 0;
  newTicketLock = {};
  masterLock = {};
}

TicketLock::Ticket TicketLock::lock() volatile {
  unsigned int ticket = getNext();
  while (ticket != threads.at(ticket % maxThreads))
    ;

  return Ticket{ticket, threads};
}

void TicketLock::unlock() volatile {
  threads = evil_increment(threads, 1, 0);
}

// slowly increment the given variable; increment and delay are both defaulted
unsigned int TicketLock::evil_increment(
    volatile unsigned int &toBeIncremented, int incrementToAdd,
    unsigned int millisecondsToHold) volatile {
  unsigned int initialValue = toBeIncremented;
  std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToHold));
  toBeIncremented = initialValue + incrementToAdd;
  return toBeIncremented;
}

unsigned int TicketLock::getNext() volatile {
  int out;
  newTicketLock.lock();
  out = newTicketUnique;
  newTicketUnique = evil_increment(newTicketUnique);
  newTicketLock.unlock();
  return out;
}
