#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef struct FileClass { // open fileClass to create file
    char file_name[80];
} FileClass;

 struct FileClass* fileList[10] = {NULL}; // create fileList to hold created files
 pthread_t threadList[5];                 // creating threadlist to hold threads
 pthread_mutex_t mutex;                   // creating mutex to lock
 pthread_mutex_t mutex1;
 char response[80] = "there is no keyword like this";

void myWrite(char *myFile, char *str){
    char *fileName = strtok(myFile, "\n"); // deleting extra line from file name
    FILE *file = fopen(fileName, "a");     // opening file with append mode
    fprintf(file,str);                     // write value to file
    fclose(file);
    strcpy(response, fileName);            // update response to send client
    strcat(response," text updated");
}

void myRead(char *myFile){
    char *fileName = strtok(myFile, "\n"); // deleting extra line from file name
    FILE *file = fopen(fileName, "r");     // opening file with reading mode
    char str[50];
    strcpy(response, fileName);
    strcat(response," have this text inside: \n");
    while (fgets(str, 50, file) != NULL) {
        strcat(response, str);             // add lines to response to send client
        printf("%s", str);
    }
    fclose(file);
}

void myCreate(char *myFile){
    char *fileName = strtok(myFile, "\n"); // deleting extra line from file name
    FILE *file = fopen(fileName, "a");     // opening file with append mode but don't write anything just create
    fclose(file);
    printf("%s",fileName);
}
void myDelete(char *myFile){
    char *fileName = strtok(myFile, "\n"); // deleting extra line from file name
    remove(fileName);                      // delete file 
    printf("%s",fileName);
}

void *listenpipe(){
    
    char * myfifo = "/tmp/file_manager_named_pipe"; // Create a named pipe 
    mkfifo(myfifo, 0666);

    char text[80];                    // create char array for hold message from client
    while(1){
    strcpy(text,"");
    pthread_mutex_lock(&mutex);       // lock for thread error
    strcpy(response,"there is no keyword like this");
    int fd1 = open(myfifo, O_RDONLY); // open fifo on read mode
    read(fd1, text, sizeof(text));    // read value and copy to text
    printf("Received: %s", text);
    close(fd1);                       // close fifo
    int textSize = 1;
    for (int i = 0; i < 80; i++){       // finding text size
        if (text[i] == ' ') { 
                textSize++;
        }
    }
    char *token = strtok(text, " ");
    char *textArray[textSize];
    int i = 0;
    while (token != NULL){              // adding text to array, split with space
        textArray[i++] = token;
        token = strtok(NULL, " ");
    }
    int control = 1;                    // create control mechanism for errors
    if(strcmp(textArray[0], "Create") == 0 && textSize == 2){           // if first word is "Create"
            char myInput[100];          // create char array for hold file name
            strcpy(myInput,strtok(textArray[1], "\n"));
            strcat(myInput,".txt");     // add .txt to end of the name
            for(int i = 0; i < 10; i++) {
                if(fileList[i] == NULL) {
                    struct FileClass *myFile = malloc(sizeof(struct FileClass));       // create file with malloc
                    strcpy(myFile->file_name, myInput);                   // add file name to file
                    fileList[i] = myFile;                                 // add file to fileList
                    myCreate(myInput);                                    // create file
                    strcpy(response, myInput);
                    strcat(response," created succesfully");
                    control = 2;                                          // update control mechanism for true
                    break;
                }
            }
            if(control == 1){                                             // if has error
                strcpy(response, myInput);
                strcat(response," can't created, don't have empty index");
            }

    } else if(strcmp(textArray[0], "Delete") == 0 && textSize == 2){      // if first word is "Delete"
            char myInput[100];          // create char array for hold file name
            strcpy(myInput,strtok(textArray[1], "\n"));
            strcat(myInput,".txt");     // add .txt to end of the name
            for(int i = 0; i < 10; i++) {
                if(fileList[i] != NULL) {
                    if(strcmp((fileList[i]->file_name),myInput) == 0) {    // check if we have in fileList
                        fileList[i] = NULL;                                // make fileList index = null
                        myDelete(myInput);
                        strcpy(response, myInput);
                        strcat(response," deleted succesfully");
                        control = 2;                                       // update control mechanism for true
                        break;
                    }
                }
            }
            if(control == 1){                                              // if has error
                strcpy(response, myInput);
                strcat(response," is not in the file list");
            }

    } else if(strcmp(textArray[0], "Read") == 0 && textSize == 2){         // if first word is "Read"
        char myInput[100];
        strcpy(myInput,strtok(textArray[1], "\n"));
        strcat(myInput,".txt");
        for(int i = 0; i < 10; i++) {
            if(fileList[i] != NULL) {
                if(strcmp((fileList[i]->file_name),myInput) == 0) {        // check if we have in fileList
                    myRead(myInput);
                    control = 2;                                           // update control mechanism for true
                    break;
                }
            }
        }
        if(control == 1){                                                  // if has error
            strcpy(response, myInput);
            strcat(response," is not in the file list");
        }
        } else if(strcmp(textArray[0], "Write") == 0 && textSize >= 3){     // if first word is "Write"
            char myInput[100];
            char str[100];
            strcpy(myInput,strtok(textArray[1], "\n"));
            strcat(myInput,".txt");
            for(int i = 0; i < 10; i++) {
                if(fileList[i] != NULL) {
                    if(strcmp((fileList[i]->file_name),myInput) == 0) {     // check if we have in fileList
                        strcpy(str,"");
                        for(int j = 0; j< textSize; j++) {
                            if(j >= 2) {
                                strcat(str,textArray[j]);
                            }
                        }
                        myWrite(myInput,str);
                        control = 2;                                        // update control mechanism for true
                        break;
                    }
                }
            }
            if(control == 1){                                               // if has error
                strcpy(response, myInput);
                strcat(response," is not in the file list");
            }
    }else if(strcmp(textArray[0], "exit\n") == 0 && textSize == 1){         // if message exit response update
            strcpy(response, "client deleted");
    }
    int fd = open(myfifo, O_WRONLY);                                        // open fifo with write mode
    write(fd, response, strlen(response)+1);                                // write response to client
    close(fd);
    pthread_mutex_unlock(&mutex);                                           // unlock mutex
    }
}

int main()
{
    pthread_mutex_init(&mutex,NULL);                                        // initilaze mutex
	for(int i=0; i<5; i++){
	    if (pthread_create(&threadList[i], NULL, listenpipe, NULL) != 0) {  // create 5 thread
	    	exit(-1);
	  	}	
    }
    
    for (int var = 0; var < 5; ++var) {
       	pthread_join(threadList[var], NULL);                                // join -wait- all threads
  	}
    return 0;
}
