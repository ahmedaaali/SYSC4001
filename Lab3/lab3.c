#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{

    void *shared_memory = (void *)0;
    int shmid;
    int random_number;
    int THRESHOLD = 50;
    int running = 1;
    pid_t child_pid;

    // create shared memory
    shmid = shmget((key_t)1234, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror(stderr, "shmget failed \n");
        exit(EXIT_FAILURE);
    }

    // create fork and attach shared memory
    child_pid = fork();
    switch (child_pid)
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        // child process
        // attach shared memory
        shared_memory = shmat(shmid, (void *)0, 0);
        if (shared_memory == (void *)-1)
        {
            perror("shmat failed \n");
            exit(1);
        }
        // generate random number
        random_number = rand() % 100;
        // write random number to shared memory
        *((int *)shared_memory) = random_number;
        // if random number is over threshold, send signal to parent
        if (random_number > THRESHOLD)
        {
            // send signal sigusr1 to parent
            kill(getppid(), SIGUSR1);
        }
        // detach shared memory
        if (shmdt(shared_memory) == -1)
        {
            perror("shmdt failed");
            exit(1);
        }
        break;
    default:
        // parent process
        // wait for signal from child
        while (running)
        {
            // wait till signal recieved from child
            pause();
            // read random number from shared memory
            printf("%d\n", *((int *)shared_memory));
            running = 0;
        }
    }
}