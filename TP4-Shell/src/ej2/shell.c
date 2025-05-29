// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <string.h>

// #define MAX_COMMANDS 200

// int main() {

//     char command[256];
//     char *commands[MAX_COMMANDS];
//     int command_count = 0;

//     while (1) 
//     {
//         printf("Shell> ");
        
//         /*Reads a line of input from the user from the standard input (stdin) and stores it in the variable command */
//         fgets(command, sizeof(command), stdin);
        
//         /* Removes the newline character (\n) from the end of the string stored in command, if present. 
//            This is done by replacing the newline character with the null character ('\0').
//            The strcspn() function returns the length of the initial segment of command that consists of 
//            characters not in the string specified in the second argument ("\n" in this case). */
//         command[strcspn(command, "\n")] = '\0';

//         /* Tokenizes the command string using the pipe character (|) as a delimiter using the strtok() function. 
//            Each resulting token is stored in the commands[] array. 
//            The strtok() function breaks the command string into tokens (substrings) separated by the pipe character |. 
//            In each iteration of the while loop, strtok() returns the next token found in command. 
//            The tokens are stored in the commands[] array, and command_count is incremented to keep track of the number of tokens found. */
//         char *token = strtok(command, "|");
//         while (token != NULL) 
//         {
//             commands[command_count++] = token;
//             token = strtok(NULL, "|");
//         }

//         /* You should start programming from here... */
//         for (int i = 0; i < command_count; i++) 
//         {
//             printf("Command %d: %s\n", i, commands[i]);
//         }    
//     }
//     return 0;
// }
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 64

char* trim(char* str) {
    char* end;
    
    // Trim leading space
    while (*str == ' ' || *str == '\t') str++;
    
    if (*str == 0) return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;
    
    end[1] = '\0';
    return str;
}

int parse_command(char* command, char** args) {
    int argc = 0;
    char* token = strtok(command, " \t");
    
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    
    args[argc] = NULL;
    return argc;
}

int handle_builtin(char** args, int argc) {
    if (argc == 0) return 0;
    
    if (strcmp(args[0], "cd") == 0) {
        if (argc < 2) {
            char* home = getenv("HOME");
            if (home && chdir(home) != 0) {
                perror("cd");
            }        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
        return 1;
    }
    else if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        return 1;
    }
    else if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}

int validate_pipeline(char** commands, int command_count) {
    if (command_count == 0) return 0;
    
    for (int i = 0; i < command_count; i++) {
        char* trimmed = trim(commands[i]);
        if (strlen(trimmed) == 0) {
            fprintf(stderr, "Syntax error: empty command in pipeline\n");
            return 0;
        }
    }
    
    return 1;
}

void handle_redirections(char** args, int* argc) {
    for (int i = 0; i < *argc - 1; i++) {
        if (strcmp(args[i], ">") == 0) {
            int fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            args[i] = NULL;
            *argc = i;
            return;
        }
    }

    for (int i = 0; i < *argc - 1; i++) {
        if (strcmp(args[i], "<") == 0) {
            int fd = open(args[i+1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            args[i] = NULL;
            *argc = i;
            return;
        }
    }
}

void execute_pipeline(char** commands, int command_count) {
    if (!validate_pipeline(commands, command_count)) {
        return;
    }
    
    if (command_count == 1) {
        char* args[MAX_ARGS];
        char command_copy[256];
        strncpy(command_copy, commands[0], sizeof(command_copy) - 1);
        command_copy[sizeof(command_copy) - 1] = '\0';
        
        char* trimmed_command = trim(command_copy);
        int argc = parse_command(trimmed_command, args);
        
        if (argc == 0) return;
        
        if (handle_builtin(args, argc)) {
            return;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            handle_redirections(args, &argc);
            execvp(args[0], args);
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        } else {
            perror("fork");
        }
        return;
    }
    
    int pipes[command_count - 1][2];
    pid_t pids[command_count];
    
    for (int i = 0; i < command_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }
    
    for (int i = 0; i < command_count; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            for (int j = 0; j < command_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return;
        }
        
        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            if (i < command_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < command_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            char* args[MAX_ARGS];
            char command_copy[256];
            strncpy(command_copy, commands[i], sizeof(command_copy) - 1);
            command_copy[sizeof(command_copy) - 1] = '\0';
            
            char* trimmed_command = trim(command_copy);
            int argc = parse_command(trimmed_command, args);
            
            if (argc == 0) {
                exit(EXIT_FAILURE);
            }
            
            if (i == command_count - 1) {
                handle_redirections(args, &argc);
            }
            
            execvp(args[0], args);
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < command_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < command_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

void process_command(const char* command) {
    char* commands[MAX_COMMANDS];
    int command_count = 0;
    
    if (command[0] == '|' || command[strlen(command)-1] == '|') {
        fprintf(stderr, "Syntax error: pipe at beginning or end\n");
        return;
    }
    
    if (strstr(command, "||") != NULL) {
        fprintf(stderr, "Syntax error: double pipe\n");
        return;
    }
    
    char command_copy[256];
    strncpy(command_copy, command, sizeof(command_copy) - 1);
    command_copy[sizeof(command_copy) - 1] = '\0';
    
    char* token = strtok(command_copy, "|");
    while (token != NULL && command_count < MAX_COMMANDS) {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }
    
    if (command_count > 0) {
        execute_pipeline(commands, command_count);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // Interactive mode
        char command[256];
        int interactive = isatty(STDIN_FILENO);

        while (1) {
            if (interactive) {
                printf("Shell> ");
                fflush(stdout);
            }
            
            if (fgets(command, sizeof(command), stdin) == NULL) {
                if (feof(stdin)) break;
                perror("fgets");
                continue;
            }
            
            command[strcspn(command, "\n")] = '\0';
            
            if (strlen(trim(command)) == 0) {
                continue;
            }
            
            if (strcmp(command, "exit") == 0) {
                break;
            }
            
            process_command(command);
        }
    } else if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        fprintf(stderr, "Usage: %s -c \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        // Non-interactive mode with command
        process_command(argv[2]);
    } else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  Interactive mode: %s\n", argv[0]);
        fprintf(stderr, "  Non-interactive mode: %s -c \"command\"\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}