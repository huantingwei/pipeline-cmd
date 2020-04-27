#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

int main() { 
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
    close(pfds[0]);
    close(pfds[1]);

    for(i=0; i<2; ++i){
        int status;
        wait(&status);
    }

   return 0; 
}
