// Program for Task 1.6

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

int signals = 0; // global variable to distinguish parent and child processes

void SIGINT_signal(int signum)
{ 
  if (signals == 0) // ignores the first signal and resumes the endless loop
  {
    printf("PID %d has recieved a SIGINT that is being ignored\n", getpid());
    // increments signals so next time it wont be ignored
    signals++;
  }
  else if (signals > 0)
  {
    // prints and terminates the program the second time SIGINT is called
    printf("PID %d has recieved a SIGINT that is being honored\n", getpid());
    exit(0); 
  }
}

int main(int argc, char **argv)
{
  pid_t fr_pid;
  int c;
  int x;

  signal(SIGINT, SIGINT_signal); 
  
  // loop to create 5 children
  for (c = 0; c < 5; c++)
  { 
    fr_pid = fork();

    // all children will exit the loop so all of the children come from the same parent
    if (fr_pid == 0)
        break;
  }

  if (fr_pid == 0) // only children can enter
  {
    while(1){sleep(100);} // endless loop, using sleep saves your CPU from 
  }                       // overheating while testing :)
    
  if (fr_pid != 0) // only the parent can enter
  {
    signals = -1; // now the parent will no longer be affected by SIGINT_signal()
    for (c = 0; c < 2; c++)
    {
      x = 5;
      while (x > 0) // count down loop
      {
        sleep(1); // actually waits a second between numbers
        printf("Countdown %d\n", x);
        x--;
      }
      kill(0, SIGINT); // sends a signal to all processes including itself (see line 56)
    }
    sleep(1); // gives the program time to print out before the terminal reprompts the user
  }
}

// BONUS: if you don't have something like sleep() in your countdown, 
// the count will probably happen faster in the parent than sending the
// child to the SIGINT_signal function. It's miniscule, but it still takes
// time to senda signal to a child and THEN have them do something, all 
// the parent needs to do is execute the next step in the loop.