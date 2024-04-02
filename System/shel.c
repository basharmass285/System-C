#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>

void Welcome() {
    printf(
        "                     aSPY//YASa\n"
        "              apyyyyCY//////////YCa       |\n"
        "             sY//////YSpcs  scpCY//Pp     | Welcome to myShell\n"
        "  ayp ayyyyyyySCP//Pp           syY//C    | Version 2.4.3\n"
        "  AYAsAYYYYYYYY///Ps              cY//S   |\n"
        "          pCCCCY//p          cSSps y//Y   | https://github.com/<user>\n"
        "          SPPPP///a          pP///AC//Y   |\n"
        "               A//A            cyP////C   | Have fun!\n"
        "               p///Ac            sC///a   |\n"
        "               P////YCpc           A//A   | Craft packets like it is your last\n"
        "        scccccp///pSP///p          p//Y   | day on earth.\n"
        "       sY/////////y  caa           S//P   |                      -- Lao-Tze\n"
        "        cayCyayP//Ya              pY/Ya   |\n"
        "         sY/PsY////YCc          aC//Yp\n"
        "          sc  sccaCY//PCypaapyCP//YSs\n"
        "                   spCPY//////YPSps\n"
        "                        ccaacs\n"
        "                                        using\n");
}

// Function to trim leading and trailing spaces
char* trimSpaces(char* str) {
    char* end;

    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return str;
}

// Enhanced getLocation Function
void getLocation() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        struct passwd *p = getpwuid(getuid());
        printf("\033[1;34mUser:\033[0m \033[1;32m%s\033[0m, \033[1;34mHost:\033[0m \033[1;32m%s\033[0m, \033[1;34mDirectory:\033[0m \033[1;32m%s\033[0m\n", p->pw_name, getenv("HOSTNAME"), cwd);
    } else {
        perror("getcwd() error");
    }
}

// Logout Function
void logout(char *str) {
    char* trimmedStr = trimSpaces(str);
    if (strcmp(trimmedStr, "exit") == 0) {
        printf("Exiting...\n");
        exit(0);
    }
}

// Split Arguments Function
char** splitArguments(char *str) {
    int spaceCount = 0;
    for (int i = 0; str[i]; i++) if (str[i] == ' ') spaceCount++;

    char** args = malloc((spaceCount + 2) * sizeof(char*)); // +2 for the first argument and NULL terminator
    int index = 0;
    char* token = strtok(str, " ");
    while (token) {
        args[index++] = token;
        token = strtok(NULL, " ");
    }
    args[index] = NULL; // NULL-terminate the array

    return args;
}

void cd(char **args) {
    char* path;
    if (args[1] == NULL) {
        // If no argument is provided, change to the home directory
        path = getenv("HOME");
        if (path == NULL) {
            fprintf(stderr, "cd: expected argument\n");
            return;
        }
    } else {
        // Use the provided argument
        path = args[1];
    }

    // Attempt to change the directory
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}


// Helper function to concatenate arguments with spaces (used in cd, cp, delete)
char* concatenateArgs(char **args) {
    static char path[1024];
    strcpy(path, ""); // Clear previous path
    for (int i = 1; args[i]; i++) {
        if (i > 1) strcat(path, " ");
        strcat(path, args[i]);
    }
    // Remove quotes if present
    if (path[0] == '\"') {
        memmove(path, path+1, strlen(path));
        path[strlen(path)-1] = '\0';
    }
    return path;
}

// File operations
void cp(char **args) {
    char *sourcePath = concatenateArgs(args);
    char *destinationPath = concatenateArgs(args + 2); // Assuming args[1] is the source and args[2] is the destination

    int source = open(sourcePath, O_RDONLY);
    int dest = open(destinationPath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    char buf[1024];
    int bytesRead;
    while ((bytesRead = read(source, buf, sizeof(buf))) > 0) {
        write(dest, buf, bytesRead);
    }

    close(source);
    close(dest);
}

void delete(char *str) {
    char* path = trimSpaces(str);
    // Remove quotes if present
    if (path[0] == '\"') {
        memmove(path, path+1, strlen(path));
        path[strlen(path)-1] = '\0';
    }
    if (unlink(path) != 0) {
        perror("delete failed");
    }
}

void move(char **args) {
    // Ensure that both source (myFile) and destination (toMove) arguments are provided
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Usage: mv <source> <destination>\n");
        return;
    }

    char *sourcePath = args[1];
    char *destinationPath = args[2];

    // Construct the new path for the file
    char newPath[1024];
    
    // Check if the destinationPath is a directory
    struct stat statbuf;
    if (stat(destinationPath, &statbuf) != -1) {
        if (S_ISDIR(statbuf.st_mode)) {
            // If the destination is a directory, append the basename of the source file
            snprintf(newPath, sizeof(newPath), "%s/%s", destinationPath, basename(sourcePath));
        } else {
            // If the destination is not a directory, it's a rename operation to a new path
            strncpy(newPath, destinationPath, sizeof(newPath));
        }
    } else {
        perror("Failed to stat destination");
        return;
    }

    // Perform the move operation
    if (rename(sourcePath, newPath) != 0) {
        perror("Failed to move file");
    }
}

// Text operations
void echo(char **args) {
  
    const char *text = args[1]; // The text to append.
    const char *filePath = args[3]; // The file path.

    // Open the file in append mode. The file is created if it does not exist.
    FILE *file = fopen(filePath, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    // Write the text to the file and add a newline character.
    fprintf(file, "%s\n", text);

    // Close the file.
    fclose(file);
}

void echorite(char **args) {
    char *text = args[1];
    char *filePath = concatenateArgs(args + 2); // Assuming args[2] is the file path

    int fd = open(filePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(fd, text, strlen(text));
    write(fd, "\n", 1);
    close(fd);
}

void vread(char **args) {
    char *filePath = concatenateArgs(args);
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        printf("File does not exist.\n");
        return;
    }

    char buf[1024];
    int bytesRead;
    while ((bytesRead = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[bytesRead] = '\0';
        printf("%s", buf);
    }
    close(fd);
}

void wordCount(char **args) {
    char *option = args[1];
    char *filePath = concatenateArgs(args + 2);
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        printf("File does not exist.\n");
        return;
    }

    char buf[1024];
    int bytesRead, lines = 0, words = 0;
    int inWord = 0;
    while ((bytesRead = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < bytesRead; ++i) {
            if (buf[i] == '\n') lines++;
            if (isspace(buf[i])) {
                if (inWord) {
                    words++;
                    inWord = 0;
                }
            } else {
                inWord = 1;
            }
        }
    }
    if (inWord) words++; // Last word might not be followed by whitespace

    if (strcmp(option, "-l") == 0) {
        printf("Lines: %d\n", lines);
    } else if (strcmp(option, "-w") == 0) {
        printf("Words: %d\n", words);
    }

    close(fd);
}
void cat(char **args) {
    int i = 1; // Start at 1 to skip the command name
    while (args[i] != NULL) { // Loop through all provided file names
        FILE *file = fopen(args[i], "r"); // Open the file for reading
        if (file == NULL) {
            perror("Error opening file");
            // Continue to the next file if this one can't be opened
            i++;
            continue;
        }

        // Read and display the file content
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }

        fclose(file); // Close the file
        i++; // Move to the next file argument
    }
}

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

