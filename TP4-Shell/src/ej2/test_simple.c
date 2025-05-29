#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_simple_command() {
    printf("Testing simple command 'ls -l'...\n");
    
    pid_t pid = fork();
    if (pid == 0) {
        execl("./shell", "shell", "-c", "ls -l", NULL);
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
    test_simple_command();
    return 0;
}