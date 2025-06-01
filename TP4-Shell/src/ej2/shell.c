#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_COMMANDS 200

char** parse_command(char* command) {
    char** args = malloc(64 * sizeof(char*));
    if (args == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int arg_count = 0;
    char* ptr = command;
    
    while (*ptr != '\0') {
        // Saltar espacios y tabs
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0') break;
        
        if (arg_count >= 64) {
            fprintf(stderr, "Too many arguments\n");
            free(args);
            exit(EXIT_FAILURE);
        }
        
        char* start = ptr;
        
        // empieza con ""
        if (*ptr == '"') {
            ptr++;
            start = ptr;
            
            while (*ptr != '\0' && *ptr != '"') {
                ptr++;
            }
            
            if (*ptr == '"') {
                *ptr = '\0';
                ptr++;
            }
        } else {
            // sin comillas
            while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t') {
                ptr++;
            }
            
            if (*ptr != '\0') {
                *ptr = '\0';
                ptr++;
            }
        }
        
        args[arg_count++] = start;
    }
    
    args[arg_count] = NULL;
    return args;
}

char* trim_whitespace(char* str) {
    if (!str) return str;

    while (*str == ' ' || *str == '\t') str++;
    if (*str == '\0') return str;

    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;
    *(end + 1) = '\0';
    return str;
}

int main() {
    char command[256];
    char *commands[MAX_COMMANDS];
    int command_count = 0;
    
    while (1) {
        if (isatty(STDIN_FILENO)) {
            printf("Shell> ");
        }
        command_count = 0;
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            if (isatty(STDIN_FILENO)) {
                printf("\n");
            }
            break;
        }
        command[strcspn(command, "\n")] = '\0';

        char *token = strtok(command, "|");
        while (token != NULL) {
            commands[command_count++] = token;
            token = strtok(NULL, "|");
        }

        for (int i = 0; i < command_count; i++) {
            commands[i] = trim_whitespace(commands[i]);
        }

        if (command_count == 0 || strlen(commands[0]) == 0) {
            continue;
        }
        
        if (command_count == 1 && strcmp(commands[0], "exit") == 0){
            if (isatty(STDIN_FILENO)) {
                printf("Goodbye!\n");
            }
            break;
        }

        // un solo comando
        if (command_count == 1) {
            
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                continue;
            }
            
            if (pid == 0) {
                // hijo
                char cmd_copy[256];
                strncpy(cmd_copy, commands[0], sizeof(cmd_copy) - 1);
                cmd_copy[sizeof(cmd_copy) - 1] = '\0';
                
                char **args = parse_command(cmd_copy);
                if (args == NULL) {
                    fprintf(stderr, "Error parsing command\n");
                    exit(EXIT_FAILURE);
                }
                
                execvp(args[0], args);
                perror("Error executing command");
                free(args);
                exit(EXIT_FAILURE);
            } else {
                // padre
                wait(NULL);
            }
        } else {
            // muchos comandos
            int pipes[command_count-1][2];
            
            for (int i = 0; i < command_count-1; i++) {
                if (pipe(pipes[i]) < 0) {
                    perror("pipe failed");
                    exit(EXIT_FAILURE);
                }
            }

            for (int i = 0; i < command_count; i++) {
                pid_t pid = fork();
                if (pid < 0) {
                    perror("fork failed");
                    exit(EXIT_FAILURE);
                }
                
                if (pid == 0) {
                    // hijo
                    
                    if (i > 0) {
                        if (dup2(pipes[i-1][0], STDIN_FILENO) < 0) {
                            perror("dup2 failed");
                            exit(EXIT_FAILURE);
                        }
                    }
                    
                    if (i < command_count - 1) {
                        if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                            perror("dup2 failed");
                            exit(EXIT_FAILURE);
                        }
                    }

                    for (int j = 0; j < command_count - 1; j++) {
                        close(pipes[j][0]);
                        close(pipes[j][1]);
                    }

                    char cmd_copy[256];
                    strncpy(cmd_copy, commands[i], sizeof(cmd_copy) - 1);
                    cmd_copy[sizeof(cmd_copy) - 1] = '\0';

                    char **args = parse_command(cmd_copy);
                    if (args == NULL) {
                        fprintf(stderr, "Error parsing command\n");
                        exit(EXIT_FAILURE);
                    }

                    execvp(args[0], args);
                    perror("Error executing command");
                    free(args);
                    exit(EXIT_FAILURE);
                }
            }

            // padre
            for (int i = 0; i < command_count - 1; i++) {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            for (int i = 0; i < command_count; i++) {
                wait(NULL);
            }
        }
    }
    return 0;
}