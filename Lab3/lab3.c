// The  program  creates  a  child  using  fork().  Add  a  piece  of  shared  memory    for  both  the  parent  and  child
// processes. The child process continuously generates a random number and puts it in the shared memory.
// If  the  random  number  is  over  a  pre-configured  threshold,  the  child  process  sends  a  signal  to  the  parent
// which  reads  the  current  value  from  the  shared  memory.  (You  will  find  examples  of  using  the  shared
// memory functions in the example provided in the book.)

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int THRESHOLD = 50;

int main()
{
    pid_t child_pid;
    char *message;
    int random_number;

    // create child process using fork()

    child_pid = fork();
    switch (child_pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        // child process
        // allocate shared memory (malloc then returns a pointer)
        message = (char *)malloc(sizeof(char) * 100);
        // generate random number
        random_number = rand() % 100;
        // write random number to shared memory
        // sprintf sends the formatted string to the message buffer
        sprintf(message, "%d", random_number);
        // if random number is over threshold, send signal to parent
        if (random_number > THRESHOLD)
        {
            // send signal sigusr1 to parent
            kill(getppid(), SIGUSR1);
        }
        // free memory
        free(message);
        break;
    default:
        // parent process
        // wait for signal from child
        wait();
        // read random number from shared memory
        printf("%s\n", message);
        break;
    }
}