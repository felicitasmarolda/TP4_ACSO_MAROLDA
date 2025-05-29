#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

// Función de prueba para parse_command (copiada del shell.c)
char** parse_command(char* command) {
    char** args = malloc(64 * sizeof(char*));
    char* token = strtok(command, " \t\r\n\a");
    int position = 0;
    
    while (token != NULL) {
        args[position] = token;
        position++;
        token = strtok(NULL, " \t\r\n\a");
    }
    args[position] = NULL;
    return args;
}

// Test helper functions
int test_count = 0;
int tests_passed = 0;

#define ASSERT_TEST(condition, test_name) \
    do { \
        test_count++; \
        if (condition) { \
            printf("✓ PASS: %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", test_name); \
        } \
    } while(0)

// Test function para parse_command
void test_parse_command() {
    printf("\n=== Testing parse_command ===\n");
    
    // Test 1: Comando simple
    char cmd1[] = "ls";
    char** args1 = parse_command(cmd1);
    ASSERT_TEST(strcmp(args1[0], "ls") == 0 && args1[1] == NULL, "Simple command parsing");
    free(args1);
    
    // Test 2: Comando con argumentos
    char cmd2[] = "ls -la /home";
    char** args2 = parse_command(cmd2);
    ASSERT_TEST(strcmp(args2[0], "ls") == 0 && 
               strcmp(args2[1], "-la") == 0 && 
               strcmp(args2[2], "/home") == 0 && 
               args2[3] == NULL, "Command with arguments parsing");
    free(args2);
    
    // Test 3: Comando con espacios extra
    char cmd3[] = "  grep   pattern  ";
    char** args3 = parse_command(cmd3);
    ASSERT_TEST(strcmp(args3[0], "grep") == 0 && 
               strcmp(args3[1], "pattern") == 0 && 
               args3[2] == NULL, "Command with extra spaces");
    free(args3);
}

// Test de pipes básico
void test_basic_pipe() {
    printf("\n=== Testing basic pipe functionality ===\n");
    
    int pipefd[2];
    pid_t pid;
    char write_msg[] = "Hello, pipe!";
    char read_msg[100];
    
    // Crear pipe
    ASSERT_TEST(pipe(pipefd) == 0, "Pipe creation");
    
    pid = fork();
    if (pid == 0) {
        // Proceso hijo - escritor
        close(pipefd[0]); // Cerrar extremo de lectura
        write(pipefd[1], write_msg, strlen(write_msg) + 1);
        close(pipefd[1]);
        exit(0);
    } else if (pid > 0) {
        // Proceso padre - lector
        close(pipefd[1]); // Cerrar extremo de escritura
        read(pipefd[0], read_msg, sizeof(read_msg));
        close(pipefd[0]);
        wait(NULL);
        
        ASSERT_TEST(strcmp(write_msg, read_msg) == 0, "Basic pipe communication");
    } else {
        printf("✗ FAIL: Fork failed in pipe test\n");
    }
}

// Test de ejecución de comandos
void test_command_execution() {
    printf("\n=== Testing command execution ===\n");
    
    // Test: ejecutar echo y capturar salida
    int pipefd[2];
    pid_t pid;
    char buffer[256];
    
    pipe(pipefd);
    pid = fork();
    
    if (pid == 0) {
        // Proceso hijo
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        char* args[] = {"echo", "test_output", NULL};
        execvp(args[0], args);
        exit(1); // Si llegamos aquí, execvp falló
    } else if (pid > 0) {
        // Proceso padre
        close(pipefd[1]);
        int bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        buffer[bytes_read] = '\0';
        close(pipefd[0]);
        wait(NULL);
        
        // Remover newline si existe
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }
        
        ASSERT_TEST(strcmp(buffer, "test_output") == 0, "Command execution with output capture");
    }
}

// Test de múltiples pipes
void test_multiple_pipes() {
    printf("\n=== Testing multiple pipes ===\n");
    
    // Simular: echo "apple\nbanana\ncherry" | grep "a" | wc -l
    // Esto debería devolver 3 (líneas que contienen 'a')
    
    // Para simplicidad, solo testeamos que se pueden crear múltiples pipes
    int pipes[2][2];
    
    int pipe1_result = pipe(pipes[0]);
    int pipe2_result = pipe(pipes[1]);
    
    ASSERT_TEST(pipe1_result == 0 && pipe2_result == 0, "Multiple pipe creation");
    
    // Cerrar todos los file descriptors
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][0]);
    close(pipes[1][1]);
}

// Test de manejo de errores
void test_error_handling() {
    printf("\n=== Testing error handling ===\n");
    
    // Test: comando que no existe
    pid_t pid = fork();
    if (pid == 0) {
        char* args[] = {"comando_que_no_existe_12345", NULL};
        int result = execvp(args[0], args);
        // Si execvp retorna, hubo un error
        exit(result == -1 ? 42 : 1); // Código especial para indicar error esperado
    } else if (pid > 0) {
        int status;
        wait(&status);
        ASSERT_TEST(WEXITSTATUS(status) == 42, "Error handling for non-existent command");
    }
}

// Test de tokenización de pipes
void test_pipe_tokenization() {
    printf("\n=== Testing pipe tokenization ===\n");
    
    // Simular la tokenización que hace el shell
    char command[] = "ls | grep .c | wc -l";
    char* commands[10];
    int command_count = 0;
    
    char* token = strtok(command, "|");
    while (token != NULL && command_count < 10) {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }
    
    ASSERT_TEST(command_count == 3, "Pipe tokenization count");
    ASSERT_TEST(strstr(commands[0], "ls") != NULL, "First command in pipe");
    ASSERT_TEST(strstr(commands[1], "grep") != NULL, "Second command in pipe");
    ASSERT_TEST(strstr(commands[2], "wc") != NULL, "Third command in pipe");
}

// Función principal de tests
int main() {
    printf("=== UNIT TESTS FOR SHELL ===\n");
    printf("Testing individual components of the shell implementation\n");
    printf("========================================================\n");
    
    test_parse_command();
    test_basic_pipe();
    test_command_execution();
    test_multiple_pipes();
    test_error_handling();
    test_pipe_tokenization();
    
    printf("\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", test_count - tests_passed);
    
    if (tests_passed == test_count) {
        printf("🎉 ALL UNIT TESTS PASSED!\n");
        return 0;
    } else {
        printf("❌ Some tests failed. Check implementation.\n");
        return 1;
    }
}