#include "ticketLock.h"

#include <chrono>
#include <thread>

TicketLock::TicketLock(unsigned int turnCount) { maxTickets = turnCount; }

TicketLock::Ticket TicketLock::lock() volatile { return Ticket{0, 0}; }

void TicketLock::unlock() volatile {}

// slowly increment the given variable; increment and delay are both defaulted
unsigned int TicketLock::evil_increment(
    volatile unsigned int &toBeIncremented, int incrementToAdd,
    unsigned int millisecondsToHold) volatile {
  unsigned int initialValue = toBeIncremented;
  std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToHold));
  toBeIncremented = initialValue + incrementToAdd;
  return toBeIncremented;
}
