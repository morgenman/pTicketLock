# TicketLock
### CIS 310: Operating Systems
### *Dylan Morgen*
### Fall 2021  
  
## Introduction
The purpose of this assignment is to implement a fair(ish) concurrency operator, a TicketLock.
It is a step above a simple SpinLock. 

## **TicketLock**
A ~TicketLock~ uses two counters, protected as necessary with ~SpinLock~, to provide mutual exclusion and fairness.
### Motivation: think of a deli counter
#### Initially: ticket (order number) is 0.
##### A thread "takes" a ticket when it wants to enter the critical region.
##### When a ticket is taken, the ticket number is incremented.
#### Initially: turn ("Now Serving" number) is 0.
##### Thread with ticket == turn can enter its critical region.
##### On exit, turn is incremented.
### Ticket and turn are incremented modulo some N.

## How Tickets are served:

|0|1|2|
|-|-|-|
|^| | |

Tickets are issued 0,1,2 as shown above.  
Should thread 0 keep running after the lock has been released, there is a good chance that thread 1 will be assigned the next ticket (0). Because thread 0 is no longer in the critical region, this is not an issue.  
Only one ticket should be able to get into the critical region, as long as the max number of threads is passed in. 
  
However, let us assume that we are trying to break this ticket lock.  

```c++
volatile TicketLock lock(2);
main(10,3); // obviously this is not how main is called, just using this as an example
```
In this situation, there would only be two ticket numbers, `0` and `1`, and thread 0 would take longer than the other threads (as presented for this assignment).

| Thread # | Iteration | Action | Ticket Issued (*italics* when not a newly assigned ticket) |
| - | - | - | - | 
| 0 | 0 | Running | 0 |
| 1 | 0 | Spinning | 1 |
| 2 | 0 | **Running** | 0 |
| 0 | 0 | Finished | *0* |
| 2 | 0 | Finished | *0* |
| 0 | 1 | Running | 1 |
| 1 | 0 | **Running** | *1* |
| ... | ... | ... | ... |

As you can see above, both bolded **Running** actions show situations where two threads could access the protected regions at the same time.  
Both have the same ticket number as a previously issued (and currently running) ticket.  

The solution to this is either to make sure that value is correct, or to use a 'forever incrementing' ticket number. You could have a simple check once you hit the max value for an unsigned int

```c++
// Placed in the relevant section
if(ticketOut == UINT_MAX) ticketOut = 0;
else ticketOut++;

// Placed in the relevant section
if(turn == UINT_MAX) turn = 0;
else turn++;
```

In my opinion, this might be a better solution, since there would be less ambiguity. It would not matter which thread is getting what. You get one ticket damnit and we are going to deal with them in order.  
I understand that *theoretically* ticket 0 could take so long that the 4294967295th ticket issued could get the same value (and thus enter the critical region at the same time), but that seems astronomically unlikely.  
  

To me, it's far more likely that the user error occurs. 

## Testing
I have tested this using a variety of values, the largest being:
```bash
./build/ticketLockTest 100 100
```
This finished for me with the last two lines of output reading:  
```bash
./build/ticketLockTest ends with (100, 100)
Final value: 10000 = 10000
```
I don't have a method for systematically testing it, but theoretically those two values should be the same as each other for any two values inputted. 
