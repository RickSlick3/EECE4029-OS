// Program for Task 1.2

#include <stdlib.h>
#include <stdio.h>

extern char **environ; // look into what “extern” means when applied to 
                       // a C global variable :)


int main(int argc, char **argv, char **envp) // using **envp replaces the need for environ
   { char **env_variable_ptr;
     
     env_variable_ptr = envp; // setting this to envp does the same as setting it to environ
                              // giving us all of the environment vars to loop through 
                              // as a pointer to a pointer to a character, so envp will 
                              // contain all of the vars
     while (*env_variable_ptr != NULL)
        { printf("%s\n", *env_variable_ptr);
          env_variable_ptr++;
        }
     
     printf("\n");

   }

