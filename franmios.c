#include "franmios.h"
#include "ppm2Ascii.h"

char *shellCommandBuiltinStr[] = {
    "help",
    "cd",
    "p2a",
    "exit"};

int shellHelp(char **args)
{
    (void)args;
    printf("**************************************************\n");
    for (long unsigned int i = 0; i < sizeof(shellCommandBuiltinStr) / sizeof(char *); i++)
    {
        printf("%s\n", shellCommandBuiltinStr[i]);
    }
    printf("**************************************************\n");
    return 1;
}

int ppm2Ascii(char **args)
{
    (void)args;
    ppm2AsciiWrapper(args[1]);
    return 1;
}

int shellChangeDirectory(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "expected argument to \"cd\"\n");
    }
    else
    {
        chdir(args[1]);
    }
    return 1;
}

int shellExit(char **args)
{
    (void)args;
    printf("EXITING...\n");
    return 0;
}

int (*shellCommandBuiltinFunc[])(char **) = {
    &shellHelp,
    &shellChangeDirectory,
    &ppm2Ascii,
    &shellExit};

char *stdinReader(void)
{
    static char buffer[BUFFER_SIZE];
    int character;
    int counter = 0;

    while (1)
    {
        character = getchar();

        if (character == '\n' || character == EOF)
        {
            buffer[counter] = '\0';
            return buffer;
        }

        buffer[counter++] = character;
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
        tokens[counter++] = token;
        token = strtok(NULL, " ");
    }

    tokens[counter] = NULL;
    return tokens;
}

int commandLauncher(char **args)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int shellExecute(char **args)
{

    if (args[0] == NULL)
    {
        return 1;
    }

    for (long unsigned int i = 0; i < sizeof(shellCommandBuiltinStr) / sizeof(char *); i++)
    {
        if (strcmp(args[0], shellCommandBuiltinStr[i]) == 0)
        {
            return (*shellCommandBuiltinFunc[i])(args);
        }
    }

    return commandLauncher(args);
}

void shellInterface(void)
{
    char *line;
    char **args;
    char currDirectory[100];
    int status = 1;

    while (status)
    {
        printf("(Franmios) - %s: ", getcwd(currDirectory, sizeof(currDirectory)));
        line = stdinReader();
        args = lineParser(line);
        status = shellExecute(args);
    }
}
