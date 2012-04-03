#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include "utils.h"

#define ARR_SIZE 1<<16




int main(int argc, char *argv[], char *envp[]){

    char buffer[BUFFER_SIZE]; //buffer for user command
    char *args[ARR_SIZE]; //splitted args
    char recentCommands[100][BUFFER_SIZE]; //a stack for last 10 commands
    int top = -1; // top of stack
    int bottom = 0; //bottom of stack
    int count; //history count
    size_t nargs; //argument count
    char command[BUFFER_SIZE]; //single string for command
    int commandNo; //command number to run from history !commandNo
    char commandToExec[BUFFER_SIZE]; //command to exec


do
{

    shellPrompt();
    commandToExec[0] = '\0';

    signal(SIGINT, handler);

    fgets(buffer, BUFFER_SIZE, stdin);

        if (strcmp(buffer, "\n") == 0) //empty command check
        continue;

    strcpy(command,buffer);
    parse_args(buffer, args, ARR_SIZE, &nargs);




        if (strcmp(args[0],"exit") == 0)
            exit(1);

        if(strcmp(args[0],"history") != 0 && args[0][0] != '!')
        historyStackPush(&top,&bottom,command,recentCommands); //push command to history


          count = 1;
          if(strcmp(args[0],"history") == 0) //list command history in descending order by time
          {

              if (nargs == 2)
                count = atoi(args[1]);
              else
                count = 10;

              if (count < 1 || count > 10)
                {
                    printf("***Please enter a number at range of 1-10***\n");
                    continue;

                }


            historyStackPop(&top, &bottom, count,recentCommands);
            continue;
          }
          else if (args[0][0] == '!')
          {
            commandNo = atoi(&args[0][1]);

            getSpecificCommand(&top, commandToExec, commandNo,recentCommands);

            if (redirect(commandToExec) != 0)
            continue;

            if (pipeCommand(commandToExec) != 0)
                continue;


            execSpecific(commandToExec);
            continue;
          }

          if (redirect(command) != 0)
            continue;



          if (pipeCommand(command) != 0)
            continue;




          execute(args);


}while (1);
    return 0;
}
