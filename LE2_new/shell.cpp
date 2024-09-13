/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

int main () {
    // lists all the files in the root directory in the long format
    char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    // translates all input from lowercase to uppercase
    char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    // TODO: add functionality
    // Create pipe
    int fid[2];
    pipe(fid);  

    // Create child to run first command
    pid_t child1 = fork();
    if(child1 == 0){ // if child is 0 then its fine
        close(fid[0]); // close input
        dup2(fid[1], STDOUT_FILENO);  // In child, redirect output to write end of pipe

        close(fid[1]); // close read end
        
        execvp(cmd1[0],cmd1); // execute command 1
    }
    

    // Create another child to run second command
    pid_t child2 = fork();
    if(child2 == 0){
        close(fid[1]);
        dup2(fid[0],STDIN_FILENO); // In child, redirect input to the read end of the pipe

        close(fid[0]); // Close the write end of the pipe on the child side.

        execvp(cmd2[0],cmd2); // Execute the second command.

    }
    // Reset the input and output file descriptors of the parent.
    close(fid[0]);
    close(fid[1]);
}
