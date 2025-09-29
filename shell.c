#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 10
#define MAX_LEN 1000
#define MAX_PIDS 5

// store last 5 child process IDs
pid_t idList[MAX_PIDS];
int idIndex = 0;

// change directory helper
void changeDir(char *path) {
    char cwd[MAX_LEN];
    if (path == NULL) {
        // cd with no args → go to HOME
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME not set\n");
            return;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return;
        }
    } else {
        if (chdir(path) != 0) {
            perror("cd");
            return;
        }
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        setenv("PWD", cwd, 1);
    }
}

// showpid helper
void showpid() {
    for (int i = 0; i < MAX_PIDS; i++) {
        if (idList[i] != 0) {
            printf("%d\n", idList[i]);
        }
    }
}

int main() {
    char buf[MAX_LEN];
    char *args[MAX_ARGS];
    char cwd[MAX_LEN];
    pid_t pid;
    int status;

    memset(idList, 0, sizeof(idList));

    while (1) {
        // print prompt in red with cwd
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("\033[0;31m%s$ \033[0m", cwd);
        } else {
            printf("prompt$ ");
        }

        if (fgets(buf, MAX_LEN, stdin) == NULL) {
            break; // EOF (Ctrl+D)
        }

        buf[strcspn(buf, "\n")] = '\0'; // remove newline

        if (strlen(buf) == 0) {
            continue; // empty input → reprompt
        }

        // tokenize input
        int i = 0;
        char *tok = strtok(buf, " ");
        while (tok != NULL && i < MAX_ARGS - 1) {
            args[i] = tok;
            i++;
            tok = strtok(NULL, " ");
        }
        args[i] = NULL;

        // built-ins
        if (strcmp(args[0], "exit") == 0) {
            printf("exit\n");
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            changeDir(args[1]);
        } else if (strcmp(args[0], "showpid") == 0) {
            showpid();
        } else {
            // external command
            pid = fork();
            if (pid == 0) {
                // child
                if (execvp(args[0], args) == -1) {
                    printf("Error: Command could not be executed\n");
                    exit(1);
                }
            } else if (pid > 0) {
                // parent
                waitpid(pid, &status, 0);
                idList[idIndex] = pid;
                idIndex = (idIndex + 1) % MAX_PIDS;
            } else {
                perror("fork failed");
            }
        }
    }
    return 0;
}
