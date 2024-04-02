#include "myShell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myFunction.h"
#include <unistd.h>
int main() {
    Welcome();
    char input[1024];

   while (1) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s$ ", cwd); // Display the current working directory followed by $ 
        } else {
            perror("getcwd() error");
            return 1; // Return an error code
        }

        if (fgets(input, sizeof(input), stdin) == NULL) break; // Exit loop on EOF (Ctrl+D)
        input[strcspn(input, "\n")] = 0; // Remove trailing newline

        if (strcmp(input, "") == 0) continue; // Skip empty inputs

        char** args = splitArguments(input);
        if (strcmp(args[0], "exit") == 0) {
            logout(input);
            break; // Break out of the loop to exit the program
        } else if (strcmp(args[0], "cd") == 0) {
            cd(args);
        } else if (strcmp(args[0], "cp") == 0) {
            cp(args);
        } else if (strcmp(args[0], "delete") == 0) {
            delete(args[1]);
        } else if (strcmp(args[0], "mv") == 0) {
            move(args);
        } else if (strcmp(args[0], "cat") == 0) {
            cat(args);
        } else if (strcmp(args[0], "echo") == 0) {
            echo(args);
        } else if (strcmp(args[0], "echorite") == 0) {
            echorite(args);
        } else if (strcmp(args[0], "vread") == 0) {
            vread(args);
        } else if (strcmp(args[0], "wordCount") == 0) {
            wordCount(args);
        } else {
            // If the command is not recognized, try to execute it as a system command
            system(input);
        }

        // Free the allocated memory for args
        free(args);
    }

    return 0;
}

