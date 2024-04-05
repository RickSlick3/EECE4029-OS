
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "RW_lock.h"

// Uncommented spin lock implementation from the lab document

int simple_spin_init(simple_spinlock_t *lock)
      {
        __asm__ __volatile__ ("" ::: "memory");
        *lock = 0;
        return 0;
      }

int simple_spin_lock(simple_spinlock_t *lock)
      {
        int i;

        while (1)
        {
          for (i=0; i < 50; i++)
            if (__sync_bool_compare_and_swap(lock, 0, 1)) return 0;

          sched_yield();
        }
      }

int simple_spin_unlock(simple_spinlock_t *lock) 
      {
        __asm__ __volatile__ ("" ::: "memory");
        *lock = 0;
        return 0;
      }

// RW lock impementation using two mutexes following the pseudo code from https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock
// I would've taken the second approach but I couldn't make the conditon part work without getting deadlocks

void RW_lock_init(RW_lock_t *lock)
      { /* This routine should be called on a pointer to a struct variable of RW_lock_t
           to initialize it and ready it for use. */

        lock->b = 0;				// counts the number of blocking readers
        simple_spin_init(&lock->g);		// initializes a simple spin lock
        pthread_mutex_init((&lock->r), NULL);	// initializes a lock from the pthread library
      }

void RW_read_lock(RW_lock_t *lock)
      { /* This routine should be called at the beginning of a READER critical section */

	// locks to protect b from race conditions
	pthread_mutex_lock(&lock->r);
	// increments the number of blocking readers
	lock->b++;
	// if there is 1 blocking reader, presumably the first, lock the global/writing mutex while threads are reading
	if (lock->b == 1)
	{
	  simple_spin_lock(&lock->g);
	}
	// unlock since end of function and b does not need to be protected
	pthread_mutex_unlock(&lock->r);
      }

void RW_read_unlock(RW_lock_t *lock)
      { /* This routine should be called at the end of a READER critical section */

	// locks to protect b from race conditions
	pthread_mutex_lock(&lock->r);
	// decrement the number of blocking readers
	lock->b--;
	// if there are no more blocking readers, the global/writing mutex doesn't need to be locked anymore
	if (lock->b == 0)
	{
	  simple_spin_unlock(&lock->g);
	}
	// unlock since end of function
	pthread_mutex_unlock(&lock->r);
	sched_yield();
      }

void RW_write_lock(RW_lock_t *lock)
      { /* This routine should be called at the beginning of a WRITER critical section */

	// lock the global/writing lock so no other readers or writers can access.
	simple_spin_lock(&lock->g);
      }

void RW_write_unlock(RW_lock_t *lock)
      { /* This routine should be called at the end of a WRITER critical section */

	// unlock the global/writing lock so another writer or reader can access.
	simple_spin_unlock(&lock->g);
      }
