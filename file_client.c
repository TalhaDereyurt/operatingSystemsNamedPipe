#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

int main(){

    char * myfifo2 = "/tmp/file_manager_named_pipe";// Created a named pipe 
    mkfifo(myfifo2, 0666);
    char text[80]; // create char array for write
    char response[80]=""; // create char array for response
    while (1)
    {
        printf("Enter text to send: ");
        fgets(text, 80, stdin);                 // get text to send server
        int fd1 = open(myfifo2, O_WRONLY);      // open fifo for write
        write(fd1, text, strlen(text)+1);       // write the text command
        close(fd1);                             // close fifo
        int fd = open(myfifo2, O_RDONLY);       // open fifo for read
        read(fd, response, sizeof(response));   // read from fifo
        close(fd);                              // close fifo
        printf("Response: %s\n", response);     // write reading value to screen
        if(strcmp(text, "exit\n") == 0){        // if command is exit, quit
            break;
        }
    }
    return 0;
}