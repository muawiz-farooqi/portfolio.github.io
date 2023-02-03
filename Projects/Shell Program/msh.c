/*

Name: Muhammad Muawiz Farooqi
ID: 1001825601

*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11    // Mav shell only supports ten arguments

#define MAX_NUM_HISTORY 15      // History will store the last 15 commands

#define MAX_NUM_PIDS 20         // Listpids will store the last 20 PIDs

int main()
{

    char *command_string = (char*) malloc(MAX_COMMAND_SIZE);
    int history_start = 0;
    int history_next = 0;
    int pids_start = 0;
    int pids_next = 0;
    char history[MAX_NUM_HISTORY][MAX_COMMAND_SIZE];
    int listpids[MAX_NUM_PIDS];

    while(1)
    {
        // Initialize empty command string
        memset(command_string, 0, MAX_COMMAND_SIZE);

        // Print out the msh prompt
        printf ("msh> ");

        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
        while(!fgets(command_string, MAX_COMMAND_SIZE, stdin));

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;

        // Initialize empty tokens
        memset(token, 0, MAX_NUM_ARGUMENTS * sizeof(char*));
                                                               
        // Pointer to point to the token
        // parsed by strsep
        char *argument_ptr;                                         
                                                               
        char *working_string  = strdup(command_string);                

        // check for ! in history
        if ('!' == command_string[0])
        {
                char *num;

                // tokenize string to get number in string format
                num = strtok(command_string, "!");

                // convert num to int, add to start of history array and MOD to keep over array indexes
                // print error message if invalid number entered
                if (!strcmp("", history[((history_start + atoi(num)) % MAX_NUM_HISTORY)]))
                {
                    printf("Command not in history.\n");
                    continue;
                }
                else
                {
                    // otherwise replace working string with command from history
                    strcpy(working_string, history[((history_start + atoi(num)) % MAX_NUM_HISTORY)]);
                }
        }

        // we are going to move the working_string pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *head_ptr = strdup(working_string);

        // Tokenize the input strings with whitespace used as the delimiter
        while (((argument_ptr = strsep(&working_string, WHITESPACE)) != NULL) && 
                  (token_count < MAX_NUM_ARGUMENTS))
        {
            token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
            if(strlen(token[token_count]) == 0)
            {
                token[token_count] = NULL;
            }
            token_count++;
        }

        // copy each command into history array to keep track of history
        strcpy(history[history_next], head_ptr);

        // check if newline present in entered command
        if (history[history_next][strlen(history[history_next])-1] == '\n')
        {
            // replace newline with null character
            history[history_next][strlen(history[history_next])-1] = '\0';
        }

        // increment start and end pointers of history array
        // use MOD operator to loop over array indexes
        history_next = (++history_next) % MAX_NUM_HISTORY;

        // if overlap between end and start then increment start
        if (history_next == history_start)
        {
            history_start = (++history_start) % MAX_NUM_HISTORY;
        }

        // check for built-in commands
        if (token[0] == NULL)
        {
            // skip/ignore empty line
        }
        else if (!strcmp("exit", token[0]) || !strcmp("quit", token[0]))
        {
            // exit program
            exit(0);
        }
        else if (!strcmp("cd", token[0]))
        {
            // change directory to second argument from command line
            chdir(token[1]);
        }
        else if (!strcmp("listpids", token[0]))
        {
            // print listpids array (last 20 pids used)
            // loop over listpids array using starting and next indexes, MOD operator to update index
            int j = 0;
            for (int i = pids_start; i != pids_next; i = ((i + 1) % MAX_NUM_PIDS))
            {
                printf("%d: %-2d\n", j++, listpids[i]);
            }
        }
        else if (!strcmp("history", token[0]))
        {
            // print history array (last 15 commands)
            // loop over history array using starting and next indexes, MOD operator to update index
            int j = 0;
            for (int i = history_start; i != history_next; i = ((i + 1) % MAX_NUM_HISTORY))
            {
                printf("[%d]: %-2s\n", j++, history[i]);
            }
        }
        else
        {
            // call fork to execute commands
            pid_t pid = fork();

            // if pid is 0 (in child)
            if(pid == 0)
            {
                // execute function store in token using execvp store result in ret
                int ret = execvp(token[0], &token[0]);

                // if ret is -1, invalid command entered, print error message
                if(ret == -1)
                {
                    printf("%s: Command not found.\n", token[0]);
                }
                return 0;
            }
            else 
            {
                // (in parent)
                // update listpids array with pid of child
                listpids[pids_next] = pid;

                // increment pid pointers for start and end of array
                pids_next = (++pids_next) % MAX_NUM_PIDS;

                // if overlap between end and start then increment start
                if (pids_next == pids_start)
                {
                    pids_start = (++pids_start) % MAX_NUM_PIDS;
                }

                // wait for child to finish executing
                int status;
                wait(&status);
            }
        }

        free(head_ptr);
    }
    return 0;
}