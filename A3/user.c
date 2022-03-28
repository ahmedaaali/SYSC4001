// User is a client process
// User is responsible for sending the commands to the text-manager
// Use "gettimeofday" to find the average time it takes to perform an operation
// print the average time required to perform the operations corresponding to the command that was sent
// System must use 2 message queues to communicate with the text-manager and the user.
// One message queue is for sending data from User to text-manager and the other is for receiving data from text-manager to User.
// Each sentence transferred must be no more than 35 characters long.

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <stdbool.h>

#define SERVER_MQUEUE 1234
#define CLIENT_MQUEUE 4321
#define MAX_TEXT 35

// Message structure to receive from text-manager
struct msg_rcv
{
    long int msg_type;
    char some_text[MAX_TEXT];
};
// Message structure to send to text-manager
struct msg_pass
{
    long int msg_type;
    // Append, Remove, Search, Delete
    char command[MAX_TEXT];
    // depends on command
    char arguement[MAX_TEXT];
};

// init message queues
static int serv_qid = -1;
static int cli_qid = -1;

int main(int argc, char const *argv[])
{

    // init
    struct msg_pass msg_pass;
    struct msg_rcv msg_rcv;
    bool is_running = true;

    // open message queues
    if ((serv_qid = msgget(SERVER_MQUEUE, 0666)) < 0)
    {
        perror("msgget (server)");
        exit(1);
    }
    if ((cli_qid = msgget(CLIENT_MQUEUE, 0666)) < 0)
    {
        perror("msgget (client)");
        exit(1);
    }

    // get user input for command
    // options are: Append, Delete, Remove, Search
    // if command is not one of the above, repeat until user enters a valid command
    // if command is Append, get user input for sentence
    // if command is Delete, get user input for word
    // if command is Remove, get user input for sentence
    // if command is Search, get user input for string

    // get user input for command
    while (1)
    {
        printf("Enter a command: (Append, Delete, Remove, Search)\n");
        scanf("%s", msg_pass.command);

        // check if user input is a valid command
        if (!strcmp(msg_pass.command, "Append") || !strcmp(msg_pass.command, "Delete") || !strcmp(msg_pass.command, "Remove") || !strcmp(msg_pass.command, "Search"))
        {
            break;
        }
        else
        {
            printf("Please enter a valid command\n");
        }
    }

    // if command is Append, get user input for sentence
    if (!strcmp(msg_pass.command, "Append"))
    {
        printf("Append Op - Enter a sentence: \n");
        scanf("%s", msg_pass.arguement);
    }
    // if command is Delete, get user input for word
    else if (!strcmp(msg_pass.command, "Delete"))
    {
        printf("Delete Op - Enter a word: \n");
        scanf("%s", msg_pass.arguement);
    }
    // if command is Remove, get user input for sentence
    else if (!strcmp(msg_pass.command, "Remove"))
    {
        printf("Remove Op - Enter a sentence: \n");
        scanf("%s", msg_pass.arguement);
    }
    // if command is Search, get user input for string
    else if (!strcmp(msg_pass.command, "Search"))
    {
        printf("Search Op - Enter a string: \n");
        scanf("%s", msg_pass.arguement);
    }

    // send message to text-manager
    if (msgsnd(serv_qid, &msg_pass, sizeof(msg_pass), 0) < 0)
    {
        perror("msgsnd");
        exit(1);
    }

    // receive message from text-manager
    if (msgrcv(cli_qid, &msg_rcv, sizeof(msg_rcv), 0, 0) < 0)
    {
        perror("msgrcv");
        exit(1);
    }

    // print the average time required to perform the operations corresponding to the command that was sent
    printf("Average time required to perform %s: %s\n", msg_pass.some_text, msg_rcv.some_text);

    // close message queues
    if (msgctl(serv_qid, IPC_RMID, NULL) < 0)
    {
        perror("msgctl (server)");
        exit(1);
    }
    if (msgctl(cli_qid, IPC_RMID, NULL) < 0)
    {
        perror("msgctl (client)");
        exit(1);
    }

    return 0;
}
