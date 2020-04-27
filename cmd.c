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
void parse_cmd(char* segments[MAX_PIPE_SEGMENTS][MAX_SEGMENT_LENGTH],char* cmdline, int* num_cmd);

int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);

    process_cmd(cmdline);
    return 0;
}

/* 
    Implementation of process_cmd

    Step 1: Parse the pipe segments and the command parameters, store in segments[][].
    Step 2: Create (num_cmd - 1) pipes. (1 pipe needed between 2 commands)
    Step 3: For each command c, create a child process
        Step 3.1: Redirect the input and output of each process
            Step 3.1.1: If c is the first command, make fin = 0, taking input from the STDIN. 
                        Otherwise, fin = pfds[c-1][0], taking input from the pipe between command c-1 and c. Make the stdin as the output from pfds[c-1][0].
            Step 3.1.2: If c is the last command, make fout = 1, outputting to the STDOUT.
                        Otherwise, fout = pfds[c][1], outputting to the pipe between command c and c+1. Make the stdout as the input of pfds[c][1].
        Step 3.2: Close all the pipes.
        Step 3.3: Execute the command.
    Step 4: Close all pipes in the parent process.
    Step 5: Wait for all children processes to terminate.

 */
void process_cmd(char* cmdline)
{
    char* segments[MAX_PIPE_SEGMENTS][MAX_SEGMENT_LENGTH];
    int num_cmd;
    parse_cmd(segments, cmdline, &num_cmd);

    int p, c;
    int pfds[num_cmd][2];
    int num_pipe = num_cmd - 1;
    // create pipes
    for(p=0; p<num_pipe; p++){
        pipe(pfds[p]);
    }
    for(c=0; c<num_cmd; c++){
        // create children process
        pid_t f = fork();
        if(f==0){
            // redirect input and output
            int fin = (c==0) ? 0 : pfds[c-1][0];
            if(fin != 0){ close(0); dup(fin); }
            int fout = (c==num_cmd-1) ? 1 : pfds[c][1];
            if(fout != 1) { close(1); dup(fout); }

            // close all the pipes
            int i;
            for(i=0; i<num_pipe; i++){
                close(pfds[i][0]);
                close(pfds[i][1]);
            }
            // execute the command
            execvp(segments[c][0], segments[c]);  
        }
    }

    // close all pipes in parent process
    for(p=0; p<num_pipe; p++){
        close(pfds[p][0]);
        close(pfds[p][1]);
    }
    // wait for the children to terminate
    for(c=0; c<num_cmd; c++){
        int status;
        wait(&status);
    }
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

// Parse the command
void parse_cmd(char* segments[MAX_PIPE_SEGMENTS][MAX_SEGMENT_LENGTH], char* cmdline, int* num_cmd){
    char* pipe_seg[MAX_PIPE_SEGMENTS];
    int num_pipe_seg;

    tokenize(pipe_seg, cmdline, &num_pipe_seg, "|");
    
    int ps, sl;
    int i; int j;
    
    for(ps=0; ps<num_pipe_seg; ps++){
        int num_arg;
        char* seg[MAX_SEGMENT_LENGTH];
        
        tokenize(seg, pipe_seg[ps], &num_arg, " ");
        
        for(sl=0;sl<num_arg;sl++){
            segments[ps][sl] = seg[sl];
        }
        
        segments[ps][num_arg] = NULL;
    }
    *num_cmd = ps;
}

// For testing: 2 level pipes using for loop
void two_level_pipe_test(){
    char* seg[2][5] = {{"ls", NULL}, {"wc", "-l", NULL}};
    
    int pfds[2];
    pipe(pfds);
    int i;
    for(i=0; i<2; i++){
        pid_t f = fork();
        if(f==0){
            close(1-i);
            dup(pfds[1-i]);
            close(pfds[i]);
            execvp(seg[i][0], seg[i]);
        }
    }

    for(i=0; i<2; i++){
        close(pfds[i]);
    }

    for(i=0; i<2; ++i){
        int status;
        wait(&status);
    }
}


