// Program for Task 1.4a

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
  pid_t fork_returned_pid;
  int count;
  int x;

  // prompts for a number from the use and sets x to the input
  printf("Gimme a number: ");
  scanf("%d", &x);

  // loop will run as many times as the users input
  for (count = 0; count < x; count++)
    { 
      // each loop creates a child process and keeps the return of fork() in fork_returned_pid
      fork_returned_pid = fork();

      // always causes the parent to wait since its fork() return is never 0
      if (fork_returned_pid != 0) wait(NULL);

      // only entered by the child because its fork() return is always 0
      if (fork_returned_pid == 0)
        {
          // prints the childs PID and terminates the child successfully using return and 
          // ending the program. 
          // exit(0) could be used instad of return    
          printf("Hello from child process %d\n", getpid());
          // once the child terminates, the parent resumes and loops again
          // creating another child, the parent waits, etc...
          return(0);
        }
    }
  // prints the parents PID only after all the children are created and terminated
  if (fork_returned_pid != 0) printf("Hello from parent process %d\n", getpid());
}