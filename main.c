#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int shellHelp(char **args);
int shellExit(char **args);

char *shellCommandBuiltinStr[] = {
    "help",
    "cd",
    "exit"};

int shellHelp(char **args)
{
    printf("**************************************************\n");
    for (int i = 0; i < sizeof(shellCommandBuiltinStr) / sizeof(char *); i++)
    {
        printf("%s\n", shellCommandBuiltinStr[i]);
    }
    printf("**************************************************\n");

    return 1;
}

int shellChangeDirectory(char **args)
{
    chdir(args[1]);
    return 1;
}

int shellExit(char **args)
{
    printf("EXITING...\n");
    return 0;
}

int (*shellCommandBuiltinFunc[])(char **) = {
    &shellHelp,
    &shellChangeDirectory,
    &shellExit};

char *stdinReader()
{
    static char buffer[BUFFER_SIZE] = "";
    int character = 0;
    int counter = 0;

    while (1)
    {
        character = getchar();

        if (character != '\n')
        {
            buffer[counter] = character;
            ++counter;
        }
        else
        {
            buffer[counter] = '\0';
            return buffer;
        }
    }
}

char **lineParser(char *line)
{
    static char *tokens[BUFFER_SIZE];
    char *token;
    int counter = 0;

    token = strtok(line, " ");

    while (token != NULL)
    {
        tokens[counter] = token;
        token = strtok(NULL, " ");
        counter++;
    }

    tokens[counter] = NULL;
    return tokens;
}

int commandLauncher(char **args)
{
    pid_t pid, ppid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // child process
        if (execvp(args[0], args) == -1)
        {
            printf("Terminated Incorrectly\n");
            return 1;
        }
    }
    else
    {
        // parent process (Shell)
        do
        {
            ppid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int shellExecute(char **args)
{
    int i;

    if (args[0] == NULL)
    {
        return 1;
    }

    for (i = 0; i < sizeof(shellCommandBuiltinStr) / sizeof(char *); i++)
    {
        if (strcmp(args[0], shellCommandBuiltinStr[i]) == 0)
        {
            return (*shellCommandBuiltinFunc[i])(args);
        }
    }

    return commandLauncher(args);
}

void shellInterface()
{
    char *line;
    char **args;
    char currDirectory[100];

    int status = 1;
    while (status)
    {
        printf("(Franmios) - %s: ", getcwd(currDirectory, 100));
        line = stdinReader();
        args = lineParser(line);
        status = shellExecute(args);
    }
}

int main()
{
    shellInterface();
    return 0;
}