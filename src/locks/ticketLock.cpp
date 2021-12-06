/**
 * Main Documentation for this file provided in header.
 */
#include "ticketLock.h"

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "SpinLock.h"

TicketLock::TicketLock(unsigned int turnCount) {
  threads = turnCount;
  turn = 0;
  ticket = 0;
}

TicketLock::Ticket TicketLock::lock() volatile {
  unsigned int ticketOut;       // Ticket it is assigned
  unsigned int serving = turn;  // Initial turn when waiting started

  spinLock.lock();
  ticketOut = ticket;  // Make sure two threads don't get the same ticket
  ticket = evil_increment(ticket) %
           threads;  // Make sure the ticket is only incremented once
  spinLock.unlock();

  while (ticketOut != turn)
    ;  // Sit and spin

  return Ticket{ticketOut, serving};
}

void TicketLock::unlock() volatile {
  // I don't believe I need to protect this because only one thread at a time
  // should be able to call it. This is because only one ticket is running in
  // the protected area at a time, and thus only one is going to call unlock at
  // a time.
  turn = evil_increment(turn) % threads;
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
