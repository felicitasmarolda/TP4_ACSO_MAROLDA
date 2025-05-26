#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define NC      "\033[0m"

void run_test(int test_num, const char *test_name, int n, int c, int s, int expected) {
    printf("%sTest %d: %s%s\n", YELLOW, test_num, test_name, NC);
    printf("Comando: ./ring %d %d %d\n", n, c, s);
    printf("Esperado: %d\n", expected);
    
    // Ejecutar el programa
    pid_t pid = fork();
    if (pid == 0) {
        // Proceso hijo
        char n_str[10], c_str[10], s_str[10];
        sprintf(n_str, "%d", n);
        sprintf(c_str, "%d", c);
        sprintf(s_str, "%d", s);
        
        execl("./ring", "./ring", n_str, c_str, s_str, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Proceso padre
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (exit_code == 0) {
                printf("%s✓ PASSED%s\n", GREEN, NC);
            } else {
                printf("%s✗ FAILED (exit code: %d)%s\n", RED, exit_code, NC);
            }
        } else {
            printf("%s✗ FAILED (no terminó normalmente)%s\n", RED, NC);
        }
    } else {
        perror("fork failed");
    }
    printf("\n");
}

int main() {
    printf("=== TESTS PARA EL ANILLO DE PROCESOS ===\n\n");
    int test_num = 1;
    
    // Ejecutar tests
    run_test(test_num++, "Caso básico (3 procesos, valor inicial 5, inicia proceso 1)", 3, 5, 1, 8);
    run_test(test_num++, "5 procesos, valor inicial 0, inicia proceso 3", 5, 0, 3, 5);
    run_test(test_num++, "1 proceso, valor inicial 1, inicia proceso 1", 1, 1, 1, 2);
    run_test(test_num++, "2 procesos, valor inicial 15, inicia proceso 1", 2, 15, 1, 17);
    run_test(test_num++, "3 procesos, valor inicial negativo (-3), inicia proceso 1", 3, -3, 1, 0);
    run_test(test_num++, "10 procesos, valor inicial 1, inicia proceso 5", 10, 1, 5, 11);
    run_test(test_num++, "6 procesos, valor inicial 7, inicia último proceso (6)", 6, 7, 6, 13);
    
    printf("=== TESTS DE CASOS LÍMITE ===\n\n");
    
    // Test de argumentos inválidos
    printf("%sTest %d: Argumentos insuficientes%s\n", YELLOW, test_num, NC);
    printf("Comando: ./ring 3 5\n");
    pid_t pid = fork();
    if (pid == 0) {
        execl("./ring", "./ring", "3", "5", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) == 0) {
            printf("%s✗ FAILED (debería haber fallado)%s\n", RED, NC);
        } else {
            printf("%s✓ PASSED (falló correctamente)%s\n", GREEN, NC);
        }
    }
    printf("\n");
    test_num++;

    printf("%sTest %d: Número de procesos inválido (0)%s\n", YELLOW, test_num, NC);
    printf("Comando: ./ring 0 5 1\n");
    pid = fork();
    if (pid == 0) {
        execl("./ring", "./ring", "0", "5", "1", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) == 0) {
            printf("%s✗ FAILED (debería haber fallado)%s\n", RED, NC);
        } else {
            printf("%s✓ PASSED (falló correctamente)%s\n", GREEN, NC);
        }
    }
    printf("\n");
    test_num++;

    printf("=== TESTS COMPLETADOS ===\n");
    
    return 0;
}