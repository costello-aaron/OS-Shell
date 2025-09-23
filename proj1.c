#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//process cmd function
int processCmd(char** cmd, char *argumentList[10]){
    printf("cmd processing..\n");
    if (strcmp(*cmd, "cd") == 0 ){
        printf("Running built in cd\n");
        //execute built in cd
        if (argumentList[1] && changeDir(argumentList[1]) == 0){
            printf("change dir successful\n");
        }else {
            printf("change dir unsuccessful\n");
        };
        //update argumentlist back to null
    } else if (strcmp(*cmd, "showpid") == 0){
        printf("Running built in showpid\n");
        //execute built in showpid
        //update argumentlist back to null
    } else {
        printf("Running %s\n", *cmd);
        //fork , exec, waitpid(), add pid to pid array
        //update argumentlist back to null
    }

    return 0;
}
//attempted changeDir just passing into chdir
int changeDir(char* path){
    printf("Change dir %s\n",path);
    return chdir(path);

}
//helper function to read out the argumentList
void printArgumentList(char *argumentList[10]) {
    for(int i = 0; i < 10 && argumentList[i] != NULL; i++) {
        printf("argumentList[%d]: %s\n", i, argumentList[i]);
    }
}

//get cmd from userInput
char* getCmd(char** cmd, char *argumentList[10]){
    char str[1000];
    char* tokenArr;

    if( fgets(str, sizeof(str), stdin) != NULL)
        {
            //remove new line char - causes a ton of issues
            str[strcspn(str, "\n")] = 0;
            printf("Start input:%s\n", str);
            tokenArr = strtok(str, " ");
            int i = 0;
            while(tokenArr != NULL)
            {
                // printf("tokenArr:%s\n", tokenArr);
                if(i == 0)
                {
                *cmd = strdup(tokenArr);
                }
            
                argumentList[i] = strdup(tokenArr);
                tokenArr = strtok(NULL, " ");
                i++;
            }
            printf("command:%s\n", *cmd);

        }
}



int main()
{
    char cwd[1000]; //stores current working dir
    char* command;  //stores command
    char* argumentList[10]; //stores

    for(int i=0; i<10; i++) // Malloc a char* pointer with length 81 and initialize it to null
    {
    argumentList[i] = (char*)malloc(sizeof(char)*81);
    argumentList[i] = NULL;
    }
    

    printf("Hello World!\n");
    

    while(1)
    {
        getcwd(cwd, sizeof(cwd));
        printf("\033[0;31m%s$ ", cwd);
        printf("\033[0m"); // Reset from red
        
        getCmd(&command, argumentList);
        printf("cmd:%s\n", command);
        printArgumentList(argumentList);
        if( strncmp(command, "exit", 4) == 0 )
        {
            printf("EXITING!\n");
            break;
        } else {
            processCmd(&command, argumentList);
        }
    }
    return 0;
}