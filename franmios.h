#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int shellHelp(char **args);
int shellChangeDirectory(char **args);
int shellExit(char **args);

char *stdinReader(void);
char **lineParser(char *line);
int commandLauncher(char **args);
int shellExecute(char **args);
void shellInterface(void);

extern char *shellCommandBuiltinStr[];
extern int (*shellCommandBuiltinFunc[])(char **);

#endif
