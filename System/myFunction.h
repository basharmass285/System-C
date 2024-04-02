#ifndef MYFUNCTION_H
#define MYFUNCTION_H

void Welcome();
char* trimSpaces(char* str);
void getLocation();
void logout(char *str);
char** splitArguments(char *str);
void cd(char **args);
char* concatenateArgs(char **args);
void cp(char **args);
void delete(char *str);
void move(char **args);
void echo(char **args);
void echorite(char **args);
void vread(char **args);
void wordCount(char **args);
void cat(char **args);

#endif /* MYFUNCTION_H */

