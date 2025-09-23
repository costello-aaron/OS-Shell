#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_LEN 1000

pid_t idList[5];
int idIndex = 0;

void chgDir(char *p, char *d) {
    if (chdir(p) == -1) {
        printf("Error: cannot change directory\n");
        return;
    }
    if (getcwd(d, 1000) == NULL) {
        perror("getcwd error");
    }
    setenv("PWD", d, 1);
}

void showpid(pid_t ids[]) {
    for (int i = 0; i < 5; i++) {
        if (ids[i] != 0) {
            printf("%d\n", ids[i]);
        }
    }
}

int main() {
    char buf[MAX_LEN];
    char *tok;
    char *args[MAX_ARGS];
    char *cmd;
    char dir[1024];
    pid_t pid;
    int st;
    int i;

    memset(idList, 0, sizeof(idList));

    while (1) {
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("\033[0;31m%s$ \033[0m", dir);
        } else {
            printf("prompt$ ");
        }

        if (fgets(buf, MAX_LEN, stdin) == NULL) {
            break;
        }
        buf[strcspn(buf, "\n")] = '\0';

        tok = strtok(buf, " ");
        i = 0;
        cmd = NULL;
        while (tok != NULL && i < MAX_ARGS - 1) {
            if (i == 0) {
                cmd = strdup(tok);
            }
            args[i] = strdup(tok);
            tok = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if (cmd == NULL) {
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            printf("exit\n");
            break;
        }
        else if (strcmp(cmd, "cd") == 0) {
            if (args[1] != NULL) {
                chgDir(args[1], dir);
            } else {
                printf("Error: cd requires a path\n");
            }
        }
        else if (strcmp(cmd, "showpid") == 0) {
            showpid(idList);
        }
        else {
            if ((pid = fork()) == 0) {
                if (execvp(cmd, args) == -1) {
                    printf("Error: Command could not be executed\n");
                    exit(1);
                }
            } else {
                idList[idIndex] = pid;
                idIndex = (idIndex + 1) % 5;
                waitpid(pid, &st, 0);
            }
        }
    }

    return 0;
}
