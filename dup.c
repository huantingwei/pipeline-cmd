#include <stdio.h> /* For print and fflush */
#include <unistd.h> /* For dup */
#include <sys/types.h>
#include <fcntl.h> /* For open syscall, flags, and user permissions */
int main() {
   int fd;
   fd = open("output.txt", /* output file name */
         O_CREAT | O_WRONLY | O_TRUNC, /* flags */
         S_IRUSR | S_IWUSR ); /* user permission: 600 */
   close(1); /* Close stdout */
   dup(fd); /* Replace stdout using the new file descriptor ID */
   printf("Hello World!\n"); /* call printf in C standard library */
   fflush(stdout); /* ensure all characters are output from the buffer */
   return 0;
}
