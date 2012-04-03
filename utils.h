#include "definitions.h"
#define ARR_SIZE 1<<16

void parse_args(char *buffer, char** args,
                size_t args_size, size_t *nargs)
{
    char *buf_args[args_size]; /* You need C99 */
    char **cp;
    char *wbuf;
    size_t i, j;


    wbuf=buffer;
    buf_args[0] = buffer;
    args[0] =buffer;

    for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;)
    {
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }

    for (j=i=0; buf_args[i]!=NULL; i++)
    {
        if(strlen(buf_args[i])>0)
            args[j++]=buf_args[i];
    }

    *nargs=j;
    args[j]=NULL;
}
void shellPrompt()
{
    printf("\n%s | #prompt>", get_current_dir_name());
}
void historyStackPush(int *top,int *bottom,char command[], char stack[][BUFFER_SIZE])
{

    (*top)++;
    strcpy(stack[*top], command);



    if (*top - *bottom > 9)
        (*bottom)++;

}
void historyStackPop(int *top, int *bottom, int count,char stack[][BUFFER_SIZE])
{

    int pull = *top;
    int i=1;

    while (count != 0 && pull >= *bottom)
    {
        printf("%d. %s",i,stack[pull]);

        count--;
        pull--;
        i++;
    }


}
void execute (char **args)
{
    pid_t pid;
    int status;

    pid=fork();
    if (pid < 0)
    {
        perror("***Fork process failed***\n");

    }
    else if (pid==0)
    {
        if (execvp(args[0],args)<0)
        {
            printf("***%s | Execution error***\n", *args);

        }
        exit(1);

    }
    else
        while (wait(&status) != pid);

}
void execSpecific(char* command)
{

    char *args[ARR_SIZE];
    size_t nargs;


    parse_args(command, args, ARR_SIZE, &nargs);
    execute(args);


}
void handler() //sigint handler
{

    printf("\nGood bye..");
    exit(getpid());
}
int redirect(char *command)
{
    int i=1;
    int directionFlag = 0;
    char *dArgs[ARR_SIZE]; //sub args for various command executions
    size_t nargs; //argument count
    char commandToExec[BUFFER_SIZE];
    char *args[ARR_SIZE];
    char tempCommand[BUFFER_SIZE];
    FILE *exists;


    strcpy(tempCommand,command);


    parse_args(tempCommand, args, ARR_SIZE, &nargs);


    strcpy(commandToExec,args[0]);



    while (args[i] != NULL) //output - input redirection
    {
        if (strcmp(args[i],">") != 0 && strcmp(args[i],"<") != 0)
        {
            strcat(commandToExec," ");
            strcat(commandToExec,args[i]);

        }
        else
        {

            if (strcmp(args[i],">") == 0)
                directionFlag = 1;
            else
                directionFlag = -1;

            break;
        }


        i++;

    }
    if (directionFlag == 1) //output
    {

        i++;

        int o = dup(fileno(stdout));

        freopen(args[i],"w+",stdout);

        parse_args(commandToExec,dArgs,ARR_SIZE,&nargs);
        execute(dArgs);



        dup2(o, fileno(stdout));
        close(o);
        return directionFlag;


    }
    else if (directionFlag == -1) //input
    {
        i++;

        int o = dup(fileno(stdin));

        if (exists=fopen(args[i],"r") == NULL)
        {
            printf("***File don't exists***");
            return 0;
        }
        else
            close(exists);


        freopen(args[i],"r",stdin);
        parse_args(commandToExec,dArgs,ARR_SIZE,&nargs);

        execute(dArgs);

        dup2(o, fileno(stdin));
        close(o);
        return directionFlag;

    }
    return directionFlag;
}
void getSpecificCommand(int *top, char *command, int commandNo, char stack[][BUFFER_SIZE])
{
    int pull= (*top)-commandNo+1;

    strcpy(command,stack[pull]);

    command[strlen(command)-1] = '\0';


}
int pipeCommand(char *command)
{
   int i=1;
    size_t nargs; //argument count
    char commandToExec[BUFFER_SIZE];
    int pipeFlag = 0;
    char commandToPipe[BUFFER_SIZE]; // command to pipe
    FILE *pp1; //left hand side of the piping
    FILE *pp2; //right hand side of the piping
    char *mystring; //output string of the piped command
    int nbytes = 100; //string size for piping
    char *args[ARR_SIZE];
    char tempCommand[BUFFER_SIZE];

    strcpy(tempCommand,command);


    parse_args(tempCommand, args, ARR_SIZE, &nargs);


    strcpy(commandToExec,args[0]);
    while (args[i] != NULL)
    {
        if (strcmp(args[i],"|") != 0)
        {
            strcat(commandToExec," ");
            strcat(commandToExec,args[i]);

        }
        else
        {
            pipeFlag = 1;
            i++;
            strcpy(commandToPipe,args[i]);
            i++;
            while (args[i] != NULL)
            {
                strcat(commandToPipe," ");
                strcat(commandToPipe, args[i]);

                i++;
            }

            break;
        }


        i++;

    }
    if (pipeFlag == 1)
    {
        pp1 = popen(commandToExec,"r");
        pp2 = popen(commandToPipe,"w");

        if (!pp1 || !pp2)
            perror("pipe error");

        mystring = (char *) malloc(nbytes+1);

        getdelim(&mystring,&nbytes,"\n\n",pp1);

        pclose(pp1);

        fprintf(pp2,"%s\n\n",mystring);
        pclose(pp2);

        return pipeFlag;


    }

    return pipeFlag;
}

