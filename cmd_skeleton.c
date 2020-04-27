#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_CMDLINE_LEN 256
#define MAX_PIPE_SEGMENTS 16
#define MAX_SEGMENT_LENGTH 256

void process_cmd(char *cmdline);
void tokenize(char **argv, char *line, int *numTokens, char *token);

int main()
{

    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);
    // use >./cmd < INPUT_FILE
    
    process_cmd(cmdline);
    return 0;
}

/* 
    Implementation of process_cmd

    TODO: Clearly explain how you implement process_cmd in point form. For example:

    Step 1: ....
    Step 2: ....
        Step 2.1: .....
        Step 2.2: .....
            Step 2.2.1: .....
    Step 3: ....

 */
void process_cmd(char* cmdline)
{
    char* pipe_seg[MAX_PIPE_SEGMENTS];
    int num_pipe_seg;

    tokenize(pipe_seg, cmdline, &num_pipe_seg, "|");
    printf("num of pipe segments: %d\n", num_pipe_seg);
    int i;
    for(i=0; i<num_pipe_seg; i++){

        execlp(pipe_seg[i], NULL);
    }



    printf("Debug: %s\n", cmdline);
}

// Implementation of tokenize function
void tokenize(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    argv[argc++] = NULL;
    *numTokens = argc - 1;
}
