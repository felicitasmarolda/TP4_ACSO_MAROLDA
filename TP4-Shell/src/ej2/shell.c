#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

// Función para parsear un comando en argumentos
char** parse_command(char* command) {
    char** args = malloc(64 * sizeof(char*)); // Array de punteros
    if (args == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int arg_count = 0;
    
    // Tokenizar por espacios
    char* token = strtok(command, " \t\n");

    while (token != NULL) {
        if (arg_count >= 63) { // Reservar espacio para el NULL final
            fprintf(stderr, "Too many arguments\n");
            free(args);
            exit(EXIT_FAILURE);
            // return NULL
        }

        args[arg_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL; // Terminar con NULL para execvp
    
    return args;
}

int main() {

    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    while (1) 
    {
        printf("Shell> ");

        command_count = 0; // cambiado por mi para reiniciar el contador
        
        /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
        fgets(command, sizeof(command), stdin);
        
        /* Removes the newline character (\n) from the end of the string stored in command, if present. 
           This is done by replacing the newline character with the null character ('\0').
           The strcspn() function returns the length of the initial segment of command that consists of 
           characters not in the string specified in the second argument ("\n" in this case). */
        command[strcspn(command, "\n")] = '\0';

        /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
           Each resulting token is stored in the commands[] array. 
           The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
           In each iteration of the while loop, strtok() returns the next token found in command. 
           The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
        char *token = strtok(command, "|");
        while (token != NULL) 
        {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        /* You should start programming from here... */


        int pipes[command_count-1][2]; // Un pipe por cada conexión entre comandos
        for (int i = 0; i < command_count-1; i++) {
            pipe(pipes[i]); // Crear cada pipe
        }

        for (int i = 0; i < command_count; i++) 
        {
            printf("Command %d: %s\n", i, commands[i]);
            pid_t pid = fork();
            if (pid == 0) {
                // Código del proceso hijo
                // Aquí configurar pipes y ejecutar comando
                // Si no es el primero: redirigir stdin desde el pipe anterior
                if (i > 0) {
                    if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) {
                        perror("dup2 failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(pipes[i-1][0], STDIN_FILENO); // Redirigir stdin al pipe anterior
                }
                // Si no es el último: redirigir stdout hacia el pipe siguiente
                if (i < command_count - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                        perror("dup2 failed");
                        exit(EXIT_FAILURE);
                    }
                    // dup2(pipes[i][1], STDOUT_FILENO); // Redirigir stdout al pipe siguiente
                }

                // cerrar todos los pipes
                for (int j = 0; j < command_count - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                // Parsear comando en argumentos
                // char *args[] = ...

                char cmd_copy[256];
                strncpy(cmd_copy, commands[i], sizeof(cmd_copy) - 1);
                cmd_copy[sizeof(cmd_copy) - 1] = '\0';

                char **args = parse_command(commands[i]);

                if (args == NULL) {
                    fprintf(stderr, "Error parsing command\n");
                    exit(EXIT_FAILURE);
                }

                // Ejecutar el comando
                execvp(args[0], args);
                perror("Error executing command");
                free(args); // Liberar memoria
                exit(EXIT_FAILURE);
            }
        }

        // PROCESO PADRE: cerrar los pipes y esperar hijos
        for (int i = 0; i < command_count - 1; i++) {
            close(pipes[i][0]); // Cerrar el extremo de lectura del pipe
            close(pipes[i][1]); // Cerrar el extremo de escritura del pipe
        }

        for (int i = 0; i < command_count; i++){
            wait(NULL);
        }


    }
    return 0;
}
