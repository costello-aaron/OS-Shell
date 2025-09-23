#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_LEN 1000
#define PID_HISTORY 5

// Track last 5 child PIDs
pid_t idList[PID_HISTORY];
int pidIndex = 0;

// Change directory
void chgDir(char *path, char *currDir) {
    if (chdir(path) == -1) {
        printf("Error: cannot change directory\n");
        return;
    }
    if (getcwd(currDir, 1000) == NULL) {
        perror("getcwd error");
    }
    setenv("PWD", currDir, 1);
}

// Show last 5 child PIDs
void showpid(pid_t idList[]) {
    for (int i = 0; i < PID_HISTORY; i++) {
        if (idList[i] != 0) {
            printf("%d\n", idList[i]);
        }
    }
}

int main() {
    char str[MAX_LEN];
    char *tokenArr;
    char *argumentList[MAX_ARGS];
    char *command;
    char currDir[1024];
    pid_t pid;
    int execStatus;
    int status;
    int i;

    memset(idList, 0, sizeof(idList));

    while (1) {
        // --- Prompt with color ---
        if (getcwd(currDir, sizeof(currDir)) != NULL) {
            printf("\033[0;31m%s$ \033[0m", currDir); // red prompt
        } else {
            printf("prompt$ ");
        }

        // --- Input ---
        if (fgets(str, MAX_LEN, stdin) == NULL) {
            break;
        }
        str[strcspn(str, "\n")] = '\0'; // strip newline

        // --- Parse ---
        tokenArr = strtok(str, " ");
        i = 0;
        command = NULL;
        while (tokenArr != NULL && i < MAX_ARGS - 1) {
            if (i == 0) {
                command = strdup(tokenArr);
            }
            argumentList[i] = strdup(tokenArr);
            tokenArr = strtok(NULL, " ");
            i++;
        }
        argumentList[i] = NULL;

        if (command == NULL) {
            continue;
        }

        // --- Built-ins ---
        if (strcmp(command, "exit") == 0) {
            printf("exit\n"); // match project spec
            break;
        }
        else if (strcmp(command, "cd") == 0) {
            if (argumentList[1] != NULL) {
                chgDir(argumentList[1], currDir);
            } else {
                printf("Error: cd requires a path\n");
            }
        }
        else if (strcmp(command, "showpid") == 0) {
            showpid(idList);
        }
        // --- External command ---
        else {
            if ((pid = fork()) == 0) {
                execStatus = execvp(command, argumentList);
                if (execStatus == -1) {
                    printf("Error: Command could not be executed\n");
                    exit(1);
                }
            }
            else {
                // Save PID in history
                idList[pidIndex] = pid;
                pidIndex = (pidIndex + 1) % PID_HISTORY;

                waitpid(pid, &status, 0);
            }
        }

        // Free memory
        for (int j = 0; j < i; j++) {
            free(argumentList[j]);
        }
        free(command);
    }

    return 0;
}
