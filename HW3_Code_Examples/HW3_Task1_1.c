// Program for Task 1.1 

#include <stdlib.h> 
#include <stdio.h> 

int main(int argc, char **argv) 
  { 
    int arg_count = 0; 

    // checks if the current string in argv is NULL
    // since argv[0] == *argv, argv[1] == *(argv+1), etc.
    // every time arg_count is incremented, the loop 
    // prints the next string in argv
    while (*(argv+arg_count) != NULL)
      {
        printf("%s\n", *(argv+arg_count)); 
        arg_count++;
      } 
  } 


