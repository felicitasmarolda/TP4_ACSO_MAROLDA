// test_ring.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void test_ring_process() {
    printf("=== Iniciando tests para ring.c ===\n");
    
    // Test 1: Anillo con 3 procesos, valor inicial 5, iniciado por proceso 1
    printf("\nTest 1: 3 procesos, valor 5, iniciado por proceso 1\n");
    if (fork() == 0) {
        execl("./ring", "ring", "3", "5", "1", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    // Test 2: Anillo con 5 procesos, valor inicial 10, iniciado por proceso 3
    printf("\nTest 2: 5 procesos, valor 10, iniciado por proceso 3\n");
    if (fork() == 0) {
        execl("./ring", "ring", "5", "10", "3", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    // Test 3: Anillo con 1 proceso (caso borde)
    printf("\nTest 3: 1 proceso, valor 7, iniciado por proceso 0\n");
    if (fork() == 0) {
        execl("./ring", "ring", "1", "7", "0", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
    
    printf("\n=== Tests completados para ring.c ===\n");
}

int main() {
    test_ring_process();
    return 0;
}