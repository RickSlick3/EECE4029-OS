#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#define MSGSIZE 1


int main(int argc, char **argv)
{
    char c;
    FILE *output, *input;

    pid_t fork_pid, wpid;
    int p[2];
    int status;
    char inbuf[MSGSIZE];

    if (pipe(p) < 0) exit(1); // create the pipe before the child

    if ((fork_pid = fork()) < 0) // fork the child and check if fork worked
    {
      printf("*** FORK ERROR ***");
      exit(0);
    }

    if (fork_pid != 0) // PARENT CODE
    {
      printf("Parent code starting\n\n"); // debug line

      close(p[0]); // close the parents read side

      input = stdin; // makes our file pointer stdin, so whatever is output before the pipe, "|",
  		     // will be read since it is considered stdin

      while (!feof(input)) // loops and gets 1 char at a time from stdin and sends it into the pipe (I know 1 char at a time isn't efficient)
      {
        c = getc(input);
        if (!feof(input)) write(p[1], &c, MSGSIZE); // sends it to the pipe
      }

      close(p[1]); // close the parents write side so the child knows to stop expecting
                   // to recieve messages to read from the pipe

      wpid = waitpid(fork_pid, &status, 0); // waits for the child to handle the information before terminating
					    // so the terminal doesn't reprompt before the info is printed

      printf("Parent code ending\n\n"); // debug line
    }

    else // CHILD CODE
    {
      printf("Child code starting\n\n"); // debug line

      close(p[1]); // close childs write side

      if (argc > 1) // if there's an argument
      {
	if (strcmp(argv[1], "-a") == 0) output = fopen(argv[2], "a"); // EXTRA CREDIT: supports the "-a" flag and will still crate a file if one does not exist in that name
								      // Just because it was easy, I don't need points for it unless you also give them to others
								      // I was just curious and finished ahead of time

        else output = fopen(argv[1], "w"); // set our other file stream to the arguments name
				           // no need to error check since the "w" option will create a file if one does not exist
				           // and will erase all contents and overwrite if one does in the same name
      }

      while (read(p[0], inbuf, MSGSIZE)) // while the child is recieving from the pipe AKA until the parent closes its writing side
      {
        printf("%s", inbuf); // prints to terminal
        if (output != NULL) // if there was a file name given as an argument, if not it will ONLY print to the terminal
        {
	  fprintf(output, "%s", inbuf); // prints inside the file we set to output
        }
      }
      printf("\n"); // for readability
      fclose(output); // closes our file stream

      printf("Child code ending\n\n"); // debug line

      close(p[2]); // close the childs read side
      exit(0); // terminates child so parent can stop waiting and finish
    }
}



