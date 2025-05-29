#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_pipe_command() {
    printf("Testing pipe command 'ls | grep .c'...\n");
    
    pid_t pid = fork();
    if (pid == 0) {
        execl("./shell", "shell", "-c", "ls | grep .c", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    printf("\nTest completed. Verify output above.\n");
}

int main() {
    test_pipe_command();
    return 0;
}