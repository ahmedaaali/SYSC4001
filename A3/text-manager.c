// text-manager stores and performs operations on text (a sequence of sentences sperated by a period)
// the text-manager is a server that receives a text from a client (user.c), performs an operation (Append, Delete, Remove, Search) and then sends the text back to the client
// System must use message queues to communicate with the text-manager and the user
// Available operations are:
// Append(sentence): A string including optional punctuation marks is sent as an argument for this command. The text-manager appends the string to the end of the text.
// Delete(W): The text-manager deletes every occurrence of the word W from the text.
// Remove(target-sentence): The text-manager removes the first occurrence of the target sentence from the text.
// Search(A): The text-manager searches for the first occurrence of the string A in the text and returns the first sentence containing it.
//

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

// Message structure to receive from user
struct msg_rcv
{
    long int msg_type;
    // Append, Remove, Search, Delete
    char command[MAX_TEXT];
    // depends on command
    char argument[MAX_TEXT];
};
// Message structure to send back to user
struct msg_pass
{
    long int msg_type;
    char time_used[MAX_TEXT];
};

// operation functions
// add sentence to end of text
void appendSentance(char *argument);
// delete word from text
void deleteWord(char *argument);
// remove sentence from text
void removeSentance(char *argument);
// search for string in text
void searchWord(char *argument);

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

    // check if input received from user.c message queue (msg_pass.command) is either Append, Delete, Remove, Search
    // if not, error and exit
    // if Append, get user input for sentence from user.c
    // if Delete, get user input for word from user.c
    // if Remove, get user input for sentence from user.c
    // if Search, get user input for string from user.c

    // get user input for command
    while (is_running)
    {
        // recieve message from user.c and store in msg_rcv
        if (msgrcv(cli_qid, &msg_rcv, sizeof(msg_rcv), 0, 0) < 0)
        {
            perror("msgrcv");
            exit(1);
        }

        printf("Command received in text-manager: %s\n", msg_rcv.command);

        // apply operation based on command received from user.c
        if (!strcmp(msg_rcv.command, "Append"))
        {
            printf("Append sentance received in text-manager: %s\n", msg_rcv.argument);
            appendSentance(msg_rcv.argument);
        }
        else if (!strcmp(msg_rcv.command, "Delete"))
        {
            printf("Delete word received in text-manager: %s\n", msg_rcv.argument);
            deleteWord(msg_rcv.argument);
        }
        else if (!strcmp(msg_rcv.command, "Remove"))
        {
            printf("Remove sentance received in text-manager: %s\n", msg_rcv.argument);
            removeSentance(msg_rcv.argument);
        }
        else if (!strcmp(msg_rcv.command, "Search"))
        {
            printf("Search string received in text-manager: %s\n", msg_rcv.argument);
            searchWord(msg_rcv.argument);
        }
        else
        {
            printf("Invalid command.\n");
            exit(1);
        }

        // send message back to user.c
    }

    return 0;
}

void appendSentance(char *argument)
{
    // append sentence to end of text
    // get length of text
    int length = strlen(text);
    // add space for new sentence
    text[length] = ' ';
    // add new sentence to end of text
    strcat(text, argument);
    // add period to end of text
    text[length + strlen(argument) + 1] = '.';
    text[length + strlen(argument) + 2] = '\0';
    // print sentance appended
    printf("Sentence appended: %s\n", argument);
}

void deleteWord(char *argument)
{
    // delete word from text
    // get length of text
    int length = strlen(text);
    // get length of word to delete
    int word_length = strlen(argument);
    // get index of first occurrence of word
    int index = strstr(text, argument) - text;
    // delete word
    for (int i = index; i < length; i++)
    {
        text[i] = text[i + word_length];
    }
    // print word deleted
    printf("Word deleted: %s\n", argument);
}

void removeSentance(char *argument)
{
    // remove sentence from text
    // get length of text
    int length = strlen(text);
    // get length of sentence to delete
    int sentance_length = strlen(argument);
    // get index of first occurrence of sentence
    int index = strstr(text, argument) - text;
    // delete sentence
    for (int i = index; i < length; i++)
    {
        text[i] = text[i + sentance_length];
    }
    // print sentence deleted
    printf("Sentence deleted: %s\n", argument);
}

void searchWord(char *argument)
{
    // search for string in text
    // get length of text
    int length = strlen(text);
    // get length of string to search for
    int string_length = strlen(argument);
    // get index of first occurrence of string
    int index = strstr(text, argument) - text;
    // get first sentence containing string
    for (int i = index; i < length; i++)
    {
        if (text[i] == '.')
        {
            text[i] = '\0';
            break;
        }
    }
    // print string found
    printf("String found: %s\n", argument);
}