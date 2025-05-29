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
    /*
    n --> número de procesos a crear
    pid --> identificador del proceso hijo
    start --> proceso que inicia la comunicación entre los procesos
    status --> almacenamos el estado de los procesos hijos
    */
    int start, status, pid, n;
    int buffer[1];      // guarda el valor entero que se transmite
    int **pipes;
    int i;

    if (argc != 4){
        printf("Uso: anillo <n> <c> <s> \n"); exit(1);
    } // antes era exit(0) --> que hace este exit?
    
    // Parsing of arguments
    n = atoi(argv[1]);      // cantidad de procesos a crear
    buffer[0] = atoi(argv[2]);  // valor inicial del buffer
    start = atoi(argv[3]);  // proceso que inicia la comunicación
    
    if (start > n || start <= 0) {
        printf("Error: El proceso inicial debe estar entre 1 y %d\n", n);
        exit(1);
    }
      printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);
    
      // un solo proceso
    if (n == 1) {
        buffer[0]++;
        printf("Resultado final: %d\n", buffer[0]);
        return 0;
    }
    
    // memoria para pipes
    pipes = (int **)malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        pipes[i] = (int *)malloc(2 * sizeof(int));
        if (pipe(pipes[i]) == -1) {
            perror("Error creating pipe");
            exit(1);
        }
    }
    
    // creamos hijos
    for (i = 0; i < n; i++) {
        if ((pid = fork()) == 0) {
            int j;
            int prev = (i == 0) ? n - 1 : i - 1;
            int next = (i + 1) % n;
            
            // cerramos los pipes que no necesitamos
            for (j = 0; j < n; j++) {
                if (j != prev) close(pipes[j][1]);
                if (j != i) close(pipes[j][0]);
            }
            
            read(pipes[i][0], buffer, sizeof(int));
            buffer[0]++;
            write(pipes[next][1], buffer, sizeof(int));
            
            // chau
            close(pipes[i][0]);
            close(pipes[prev][1]);
            exit(0);
        }
    }
    
    // padre
    for (i = 0; i < n; i++) {
        if (i != start - 1) close(pipes[i][0]);
        if (i != ((start - 2 + n) % n)) close(pipes[i][1]);
    }
    
    // valor inicial al proceso que inicia
    write(pipes[(start - 2 + n) % n][1], buffer, sizeof(int));
    
    // valor final del último proceso
    read(pipes[start - 1][0], buffer, sizeof(int));
    
    close(pipes[(start - 2 + n) % n][1]);
    close(pipes[start - 1][0]);
    
    for (i = 0; i < n; i++) {
        wait(&status);
    }
    
    for (i = 0; i < n; i++) {
        free(pipes[i]);
    }
    free(pipes);
    
    printf("Resultado final: %d\n", buffer[0]);
    
    return 0;
}
