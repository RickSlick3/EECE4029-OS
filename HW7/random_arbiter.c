// This is an example of standard dining philosophers.  This version has NO deadlock
// prevention.  Guess what it will do?

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


// Change the constant below to change the number of philosophers
// coming to lunch...

#define PHILOSOPHER_COUNT 10

// Change the constant below to change how many "meals" are on the table.  This is 
// the number of times that any philosopher can eat.  Once all the meals are gone, then
// the philosophers are sent home (the threads terminate).  Note it MAY be the case
// that the deadlock prevention scheme is NOT fair.  It may be possible that 
// not all philosophers get to eat the same amount of meals....

#define AVAILABLE_MEALS 1000000

// Each philosopher is represented by one thread.  Each thread independenly
// runs the same "think/start eating/finish eating" program.

pthread_t philosopher[PHILOSOPHER_COUNT];

// Each chopstick gets one mutex.  If there are N philosophers, there are
// N chopsticks.  That's the whole problem.  There's not enough chopsticks 
// for all of them to be eating at the same time.  If they all cooperate, 
// everyone can eat.  If they don't... or don't know how.... well.... 
// philosophers are going to starve.

pthread_mutex_t chopstick[PHILOSOPHER_COUNT];

// The next item is a global variable that keeps track of how many meals 
// have been eaten.  WHen it goes to zero, all the food is gone (so sad)
// and the threads exit.  Because this is a global variable that is accessed
// by multiple threads, I will also provide a mutex for it so I don't get 
// a race condition.  THIS IS NOT PART OF ANY DEADLOCK PREVENTION SCHEME.

int MEALS_ON_TABLE = AVAILABLE_MEALS;
pthread_mutex_t MEAL_COUNT_LOCK;

// MUTEX FOR THE ARBITER SOLUTION, THIS WILL BE USED SO ONLY ONE PHILOSOPHER
// CAN PICK UP CHOPSTICKS AT ONCE (ACCESS THE CHOPSTICK MUTEX), AND WILL PICK
// UP BOTH CHOPSTICKS BEFORE THE NEXT PHILOSOPHER CAN START PICKING UP CHOPSTICKS

pthread_mutex_t ARBITER;

void *philosopher_program(int philosopher_number)
   {  
      int how_many_times_did_I_eat = 0;
      
      int chopstick_1;
      int chopstick_2;
      
      while (MEALS_ON_TABLE > 0)
         { // In this version of the "philosopher program", the philosopher
           // will think and eat until all the meals are gone
     
           printf ("Philosopher %d is thinking\n", philosopher_number);
           usleep(1);  
           
           // That was a lot of thinking.... now hungry... this 
           // philosopher (who knows his own number) THIS guy decides 
           // he wants two chopsticks at random.  He will at least choose
           // two DIFFERENT chopsticks
           
           chopstick_1 = rand() % PHILOSOPHER_COUNT;
           do  
             chopstick_2 = rand() % PHILOSOPHER_COUNT;
           while (chopstick_1 == chopstick_2);
           
           printf ("Philosopher %d wants chopsticks %d and %d\n",philosopher_number, chopstick_1, chopstick_2);
    
	   // THE ARBITER COMES TO SERVE THE NEXT PHILOSOPHER IN LINE TO GIVE THEM THEIR CHOPSTICKS
           // THE ARBITER LOCKS HERE SO THAT NO OTHER PHILOSOPHER CAN TRY TO PICK UP CHOPSTICKS UNTIL
           // THE CURRENT PHILOSOPHER HAS BOTH OF THEIRS

           // USE THE ARBITER BEFORE ACCESSING THE CHOPSTICKS

	   pthread_mutex_lock(&ARBITER);

	   // IF THE PHILOSOPHER BEFORE THE CURRENT ONE BEING WAITED ON IS HOLDING EITHER OF THE
	   // CHOPSTICKS THAT THE CURRENT ONE NEEDS, THEN THE CURRENT PHILOSOPHER WILL HAVE TO WAIT 
	   // FOR THEM TO PUT DOWN THEIR CHOPSTICKS BEFORE MOVING ON.

           // THIS MEANS THAT NO OTHER PHILOSOPHER CAN BE WAITED ON UNTIL BOTH CHOPSTICKS ARE AVAILIBLE

           // OTHERWISE, IF BOTH ARE AVAILIBLE THEN THE PHILOSOPHER CAN PICK THEM UP AND SEND THE WAITER 
           // BACK TO THE NEXT PHILOSOPHER

           pthread_mutex_lock(&chopstick[chopstick_1]);
           pthread_mutex_lock(&chopstick[chopstick_2]);

	   // ONCE THEY HAVE THEIR CHOPSTICKS, THE ARBITER UNLOCKS AND GOES TO SERVE THE NEXT PHILOSOPHER
           // THE ARBITER CAN UNLOCK HERE BECAUSE THE CURRENT PHILOSOPHER WOULD HAVE BOTH OF THEIR CHOPSTICKS
           // AT THIS POINT IN THE PROGRAM

	   // UNLOCK THE ARBITER SINCE BOTH CHOPSTICKS ARE PICKED UP

	   pthread_mutex_unlock(&ARBITER);

           // Hurray, if I got this far I'm eating
           printf ("Philosopher %d is ready to eat\n",philosopher_number);

           usleep(2); // I spend twice as much time eating as thinking...
                      // typical....
           
           // I'm done eating.  Now put the chopsticks back on the table

           if (MEALS_ON_TABLE > 0)
              { how_many_times_did_I_eat++;
                pthread_mutex_lock(&MEAL_COUNT_LOCK);
                MEALS_ON_TABLE--;
                printf ("Philosopher %d finished eating, %d meals remaining\n",philosopher_number, MEALS_ON_TABLE);
                pthread_mutex_unlock(&MEAL_COUNT_LOCK);

              }
 
           else
              { printf("Philosopher %d didn't eat because no food was left\n", philosopher_number);
              }
              
           pthread_mutex_unlock(&chopstick[chopstick_1]);
           pthread_mutex_unlock(&chopstick[chopstick_2]);
           printf("Philosopher %d has placed chopsticks on the table\n", philosopher_number);
         } 

      printf("                              PHILOSOPHER %d GOT TO EAT %d TIMES\n", philosopher_number, how_many_times_did_I_eat);
      return(NULL);
     }

int main()
  {  int i;


   // Seed the random number generator
   srand(time(NULL));
   
   // Initialize the MEALS_ON_TABLE variable locking mutex
   pthread_mutex_init(&MEAL_COUNT_LOCK, NULL);
   
   // Initialize all the chopsticks as "unlocked" or "not held"
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_mutex_init(&chopstick[i],NULL);

   // INITIALIZE THE 'ARBITER' LOCKING MUTEX
   pthread_mutex_init(&ARBITER, NULL);

   // Create the philosopher threads  
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_create(&philosopher[i],NULL,(void *)philosopher_program,(void *)i);

   // collect the philosophers when they are done....  if the philosopher threads 
   // never terminate, then these joins never happen.
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_join(philosopher[i],NULL);

   // Once all the threads terminate, destroy all the chopsticks
   for(i=0;i<PHILOSOPHER_COUNT;i++)
      pthread_mutex_destroy(&chopstick[i]);

   return 0;
   }

