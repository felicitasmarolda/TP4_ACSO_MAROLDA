#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void run_shell(const char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        execl("./shell", "shell", "-c", command, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

void test_quoted_command() {
    printf("Testing quoted command 'ls | grep \".c\"'...\n");
    run_shell("ls | grep \".c\"");
    printf("\nTest passed if only .c files were listed above.\n");
}

int main() {
    test_quoted_command();
    return 0;
}