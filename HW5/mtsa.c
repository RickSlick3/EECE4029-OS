#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>  // This is the header file that contains prototypes for
                      // pthreads structures and user functions

int original[40];  // global variables so the memory for this variable
                   // will be in the process DATA segment and will have global scope.
int sorted[40];    // original will be filled in main and sorted will be filled in
		   // the sorting thread.

// struct to hold start and end index and array half the size of the original that 
// we will sort

typedef struct arg1
          { int start_index;  // Input parameter
            int end_index;    // Input parameter
            int sorted_half[20];  // Space to put the sorted half-array
          }  arg1_type;

typedef arg1_type *arg1_ptr;  // defining/naming the pointer to the type above

// struct to take the two sorted halves of the array into the third thread

typedef struct arg2
	  { int half1[20];
	    int half2[20];
          }  arg2_type;

typedef arg2_type *arg2_ptr;  // defining/naming the pointer to the struct


void *sort_half(void *input)
{
    // LOCAL TO THE THREAD variables
    int i, j, min_index, temp;
    int start_index;
    int end_index;
    int arr[20];

    start_index = ((arg1_ptr)input)->start_index; // making a copy of the struct fields
    end_index   = ((arg1_ptr)input)->end_index;

    // filling our local array with the values that a thread needs to sort based on its
    // start and end index

    // this will access the global unsorted array to set the values of our local half array

    for(i=start_index; i <= end_index; i++)
      arr[i%20] = original[i];              // using i%20 to properly assign indexes in arr

    // SELECTION SORT
    for (i = 0; i < 19; i++)
    {
      min_index = i;
      for (j = i+1; j < 20; j++)
      {
        if (arr[j] < arr[min_index])
          min_index = j;
      }

      if(min_index != i)
      { temp = arr[min_index];
        arr[min_index] = arr[i];
        arr[i] = temp;
      }
    }

    for(i=0; i < 20; i++)
    {
      ((arg1_ptr)input)->sorted_half[i] = arr[i]; // seting the struct value to our local sorted array
    }

    pthread_exit(0); // thread returning to main thread
}


void *combine_sort(void *input)
{
    int i, x, y; // LOCAL TO THE THREAD variables

    x = 0;
    y = 0;

    // looping through all indexes of out sorted array to inject our sorted values into
    // this will be assigning indexes the smallest unused value between the two arrays
    for (i=0; i < 40; i++)
    {
      // if we have not taken all the values from either of the arrays we need to check both
      // for the smaller value to put into our soted array first
      if (x < 20 && y < 20)
      {
        if (((arg2_ptr)input)->half1[x] < ((arg2_ptr)input)->half2[y])
        { sorted[i] = ((arg2_ptr)input)->half1[x];
          x++; // instead of removing values, we just track the index of the smallest value 
        }      // that we haven't added to the sorted array in this half (same for y++)
        else
        { sorted[i] = ((arg2_ptr)input)->half2[y];
          y++;
        }
      }
      // if one of the arrays is empty we need to find out which one
      // then we can just keep adding the variables that are left to the sorted array
      else if (x < 20)
      { sorted[i] = ((arg2_ptr)input)->half1[x];
        x++;
      }
      else
      { sorted[i] = ((arg2_ptr)input)->half2[y];
        y++;
      }
    }
    // at this point our global sorted variable should be full of sorted value and the thread can exit
    pthread_exit(0);
}


int main()
{
    int i, j;  // counter for local loops

    int rt1, rt2, rt3;  // return values for when we launch the threads

    pthread_t t1, t2, t3;  // book keeping values for the 3 threads

    arg1_type args_1, args_2;  // blank structs to hold data for first 
			       // two threads

    arg2_type args_3; // blank struct to hold data for the third thread

    // filling original[] with random values from 1-100 and printing the unsorted array

    printf("\nfilling the original array\n\n");
    time_t t;
    srand((unsigned)time(&t)); 
    for (i=0; i < 40; i++)
    {
      original[i] = (int)((rand()%100));
      printf("%d ", original[i]);
    }

    // filling the start and end indexes for the first two structs

    args_1.start_index = 0;
    args_1.end_index   = 19;
    args_2.start_index = 20;
    args_2.end_index   = 39;

    // making sure the threads are created successfully

    if((rt1=pthread_create( &t1, NULL, sort_half, (void *)&args_1))) 
       printf("Thread creation failed: %d\n", rt1);

    if((rt2=pthread_create( &t2, NULL, sort_half, (void *)&args_2)))
       printf("Thread creation failed: %d\n", rt2);

    // having the threads rejoin the parent thread when they exit

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // taking the sorted halves and putting them into the second struct

    for (i=0; i < 20; i++)
    {
      args_3.half1[i] = args_1.sorted_half[i];
      args_3.half2[i] = args_2.sorted_half[i];
    }

    // making sure the 3rd thread is created successfully

    if((rt3=pthread_create( &t3, NULL, combine_sort, (void *)&args_3))) 
       printf("Thread creation failed: %d\n", rt3);

    // having the third thread rejoin the parent here

    pthread_join(t3, NULL);

    // printing out the sorted array

    printf("\n\nPrinting the sorted array\n\n");

    for (i=0; i < 40; i++)
        printf("%d ", sorted[i]);

    printf("\n\n");

    return 0;
}
