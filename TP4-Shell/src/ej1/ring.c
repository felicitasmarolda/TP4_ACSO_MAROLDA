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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) { 
    
    // devuelve la cantidad de veces que cambio de proceso + c

    int n, c, s;
    int pid, status;
    int **pipes;

    if (argc != 4) {
        printf("Uso: anillo <n> <c> <s>\n");
        exit(1);
    }

    n = atoi(argv[1]);
    c = atoi(argv[2]);
    s = atoi(argv[3]);    
    
    if (n < 3) {
        printf("Error: <n> debe ser mayor igual a 3.\n");
        exit(1);
    }

    // if (c < 0) {
    //     printf("Error: <c> debe ser un entero no negativo.\n");
    //     exit(1);
    // }

    if (s < 1 || s > n) {
        printf("Error: <s> debe estar entre 1 y %d\n", n);
        exit(1);
    }

    printf("Se crearán %d procesos, se enviará el entero %d desde proceso %d\n", n, c, s);

    // creo la amtriz para escritura y lectura
    pipes = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        pipes[i] = (int *)malloc(2 * sizeof(int));
        if (pipe(pipes[i]) == -1) {
            perror("Error al crear pipe");
            exit(1);
        }
    } 

    for (int i = 0; i < n; i++) {
        if ((pid = fork()) == 0) {
            int prev = (i == 0) ? n - 1 : i - 1;
            int next = (i + 1) % n;

            for (int j = 0; j < n; j++) {
                if (j != prev) close(pipes[j][1]);
                if (j != i) close(pipes[j][0]);
            }

            if (i == s - 1) {
                c++;
                write(pipes[next][1], &c, sizeof(int));

            } else {
                read(pipes[i][0], &c, sizeof(int));
                c++;
                write(pipes[next][1], &c, sizeof(int));
            }

            close(pipes[i][0]);
            close(pipes[prev][1]);
            exit(0);
        }
    } 

    for (int i = 0; i < n; i++) {
        if (i != s - 1) close(pipes[i][0]);
        close(pipes[i][1]); 
    }

    read(pipes[s - 1][0], &c, sizeof(int));

    close(pipes[s - 1][0]);

    for (int i = 0; i < n; i++) {
        wait(&status);
    }

    // Liberar memoria de los pipes.
    for (int i = 0; i < n; i++) {
        free(pipes[i]);
    }
    free(pipes);

    // Mostrar resultado.
    printf("Resultado final: %d\n", c);

    return 0;
}