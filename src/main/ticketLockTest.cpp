/**
 * The test frame work for the ticket lock implementation.
 */
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "ticketLock.h"

using namespace std;

// loop iteration counter
volatile unsigned int counter = 0;

// default values
constexpr int DEFAULT_ITERATIONS = 10;
constexpr int DEFAULT_THREADS = 3;

/**
 * Pause (sleep) the current thread for a uniformly distributed time
 * between 0 and max_nanoseconds nanoseconds.
 *
 * @param min_nanoseconds minimum time to sleep (in nanoseconds)
 * @param max_nanoseconds maximum time to sleep (in nanoseconds)
 * @param generator a randomness generator per C++17
 */
static inline void random_pause(long min_nanoseconds, long max_nanoseconds,
                                mt19937 &generator) {
  uniform_int_distribution<> distribution(min_nanoseconds, max_nanoseconds);
  timespec t;
  t.tv_sec = 0;
  t.tv_nsec = distribution(generator);
  nanosleep(&t, NULL);
}

/**
 * The worker function for our threads. The thread is started with its
 * name, the number of iterations it will run, and a reference to the
 * shared TicketLock.
 *
 * A thread-local randomness generator (gen) is constructed; this
 * makes it thread-safe to generate a sequence of random numbers (the
 * actual sleep values).
 *
 * The for loop iterates the global counter by making a local copy,
 * incrementing it, and then writing it back. This purposly increases
 * the chance of a thread context-switch happening between these
 * different steps. The random pause calls pause each thread for a
 * random amount of time to keep the execution of the threads from
 * being in lockstep; this makes the TicketLock important.
 *
 * @param threadName the name this thread will print with
 * @param loops the number of times the for loop will be run
 * @param ticket_lock pointer to the _shared_ TicketLock object that
 *        coordinates the threads' access to counter
 */
void thread_worker(const string &threadName, int loops,
                   volatile TicketLock &ticket_lock, int threadNumber) {
  random_device
      rd;  // Will be used to obtain a seed for the random number engine
  mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()

  for (int i = 0; i < loops; i++) {
    TicketLock::Ticket ticket = ticket_lock.lock();
    int local_copy_of_counter = counter;
    cout << threadName << "[" << i
         << "]: counter (ticket/turn): " << local_copy_of_counter << "("
         << ticket.ticket << "/" << ticket.initial_turn << ")" << endl;
    local_copy_of_counter++;
    counter = local_copy_of_counter;
    random_pause(10000000L, 15000000L, gen);
    ticket_lock.unlock();
    if (threadNumber == 0) {
      timespec t;
      t.tv_sec = 1;  // TODO change this back to 4
      t.tv_nsec = 0;
      nanosleep(&t, NULL);
    }
    random_pause(100000000L, 150000000L, gen);
  }
  cout << threadName << ": done" << endl;
}

/**
 * Display the usage message on standard error
 * @param executable_name name of command run to start this function
 * @param iterations default value of the iteration count
 * @param threads default value of the threads
 */
void usage(string executable_name, int iterations, int threads) {
  cerr << "usage: " << executable_name << " <iterations> <threads>" << endl
       << "       <iterations> - number of iterations per thread ("
       << iterations << ")" << endl
       << "       <threads> - number of threads to start (" << threads << ")"
       << endl
       << "       both parameters are optional and default to the shown values."
       << endl;
  exit(1);
}

/**
 * The main method. Processes command-line parameters then launches
 * the given number of worker threads (an array of thread pointers and
 * a loop), and, finally, joins each of the threads in sequential
 * order.
 */
int main(int argc, char *argv[]) {
  vector<string> arguments;  // command-line arguments translated to strings
  for (int aNdx = 0; aNdx < argc; ++aNdx) arguments.push_back(argv[aNdx]);

  unsigned int number_of_iterations =
      (arguments.size() > 1) ? stoi(arguments[1]) : DEFAULT_ITERATIONS;
  unsigned int number_of_threads =
      (arguments.size() > 2) ? stoi(arguments[2]) : DEFAULT_THREADS;
  unsigned int expected_value = number_of_iterations * number_of_threads;

  cout << arguments[0] << " starts with (" << number_of_iterations << ", "
       << number_of_threads << ")" << endl;

  // The TicketLock is volatile to make sure the compiler knows it
  // might be changed by another thread.
  volatile TicketLock lock(number_of_threads);

  // dynamically allocate array of thread pointers
  thread **threads = new thread *[number_of_threads];

  // create threads; keep pointers to them in the threads array
  for (unsigned int i = 0; i < number_of_threads; ++i) {
    string name = "worker_thread " + to_string(i);
    threads[i] =
        new thread(&thread_worker, name, number_of_iterations, ref(lock), i);
  }

  // join the threads
  for (unsigned int i = 0; i < number_of_threads; ++i) {
    cout << "main: joining worker_thread " << i << endl;
    threads[i]->join();
    cout << "main: joined worker_thread " << i << endl;
  }

  // final message
  cout << arguments[0];
  cout << " ends with (";
  cout << number_of_iterations;
  cout << ", ";
  cout << number_of_threads;
  cout << ")" << endl;
  cout << "Final value: " << counter
       << ((counter == expected_value) ? " = " : " < ") << expected_value
       << endl;

  return 0;
}
