#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


// int main(int argc, char **argv)
// {	
// 	int start, status, pid, n;
// 	int buffer[1];

// 	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
//     /* Parsing of arguments */
//   	/* TO COMPLETE */
//     printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
//    	/* You should start programming from here... */
// }

int main(int argc, char **argv)
{	
    int start, status, pid, n;
    int buffer[1];
    int **pipes;
    int i;

    if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    /* Parsing of arguments */
    n = atoi(argv[1]);      // Number of processes
    buffer[0] = atoi(argv[2]);  // Initial message value
    start = atoi(argv[3]);  // Process that starts communication
    
    if (start > n || start <= 0) {
        printf("Error: El proceso inicial debe estar entre 1 y %d\n", n);
        exit(1);
    }
    
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
    /* Creating pipes for communication */
    pipes = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        pipes[i] = (int *)malloc(2 * sizeof(int));
        if (pipe(pipes[i]) == -1) {
            perror("Error creating pipe");
            exit(1);
        }
    }
    
    /* Creating child processes */
    for (i = 0; i < n; i++) {
        if ((pid = fork()) == 0) {
            int j;
            int prev = (i == 0) ? n - 1 : i - 1;  // Previous process in the ring
            int next = (i + 1) % n;               // Next process in the ring
            
            /* Close all unused pipe ends */
            for (j = 0; j < n; j++) {
                if (j != prev) close(pipes[j][1]);  // Close write ends not used by this process
                if (j != i) close(pipes[j][0]);     // Close read ends not used by this process
            }
            
            /* Process logic: read, increment, and send */
            read(pipes[i][0], buffer, sizeof(int));
            buffer[0]++;
            write(pipes[next][1], buffer, sizeof(int));
            
            /* Clean up and exit */
            close(pipes[i][0]);
            close(pipes[prev][1]);
            exit(0);
        }
    }
    
    /* Parent process code */
    /* Close all pipes except the ones needed for start and end communication */
    for (i = 0; i < n; i++) {
        if (i != start - 1) close(pipes[i][0]);
        if (i != ((start - 2 + n) % n)) close(pipes[i][1]);  // Previous process to start
    }
    
    /* Send initial value to the starting process */
    write(pipes[(start - 2 + n) % n][1], buffer, sizeof(int));
    
    /* Receive final value from the last process */
    read(pipes[start - 1][0], buffer, sizeof(int));
    
    /* Clean up remaining pipes */
    close(pipes[(start - 2 + n) % n][1]);
    close(pipes[start - 1][0]);
    
    /* Wait for all children to finish */
    for (i = 0; i < n; i++) {
        wait(&status);
    }
    
    /* Free allocated memory */
    for (i = 0; i < n; i++) {
        free(pipes[i]);
    }
    free(pipes);
    
    /* Print final result */
    printf("Resultado final: %d\n", buffer[0]);
    
    return 0;
}
