// test_shell.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void test_shell_commands() {
    printf("=== Iniciando tests para shell.c ===\n");
    
    // Test 1: Comando simple con pipe
    printf("\nTest 1: ls | grep .c\n");
    if (fork() == 0) {
        execl("./shell", "shell", "ls | grep .c", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    // Test 2: Comando con múltiples pipes
    printf("\nTest 2: ls -l | grep .c | wc -l\n");
    if (fork() == 0) {
        execl("./shell", "shell", "ls -l | grep .c | wc -l", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    // Test 3: Comando sin pipes
    printf("\nTest 3: ls -l\n");
    if (fork() == 0) {
        execl("./shell", "shell", "ls -l", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    // Test 4: Comando con comillas (extra credit)
    printf("\nTest 4: ls | grep \".c .h\"\n");
    if (fork() == 0) {
        execl("./shell", "shell", "ls | grep \".c .h\"", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    printf("\n=== Tests completados para shell.c ===\n");
}

int main() {
    test_shell_commands();
    return 0;
}