#include <unistd.h>
#include <signal.h>
#define BUFFER_SIZE 1<<16

void parse_args(char *buffer, char** args,
                size_t args_size, size_t *nargs);
void shellPrompt();

//history stack
void historyStackPush(int *top, int *bottom,char *command, char stack[][BUFFER_SIZE]);
void historyStackPop(int *top, int *bottom, int count, char stack[][BUFFER_SIZE]);
void execute (char **args);
void execSpecific(char* command);
int redirect(char *commandToExec);
void getSpecificCommand(int *top, char *command, int commandNo, char stack[][BUFFER_SIZE]);
