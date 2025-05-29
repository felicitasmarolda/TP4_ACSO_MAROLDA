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

// int main(int argc, char **argv)
// {	
//     /*
//     n --> número de procesos a crear
//     pid --> identificador del proceso hijo
//     start --> proceso que inicia la comunicación entre los procesos
//     status --> almacenamos el estado de los procesos hijos
//     */
//     int n, c, s;  // n = número de procesos, c = entero a transmitir, s = proceso inicial.
//     int pid, status;
//     int **pipes;

//     // int s, status, pid, n;
//     // int buffer[1];      // guarda el valor entero que se transmite
//     // int **pipes;
//     int i;

//     if (argc != 4){
//         printf("Uso: anillo <n> <c> <s> \n"); exit(1);
//     }
    
//     n = atoi(argv[1]);      // cantidad de procesos a crear
//     buffer[0] = atoi(argv[2]);  // valor inicial del buffer
//     s = atoi(argv[3]);  // proceso que inicia la comunicación
    
//     if (s > n || s <= 0) {
//         printf("Error: El proceso inicial debe estar entre 1 y %d\n", n);
//         exit(1);
//     }
//       printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], s);
    
//       // un solo proceso
//     if (n == 1) {
//         buffer[0]++;
//         printf("Resultado final: %d\n", buffer[0]);
//         return 0;
//     }
    
//     // memoria para pipes
//     pipes = (int **)malloc(n * sizeof(int *));
//     for (i = 0; i < n; i++) {
//         pipes[i] = (int *)malloc(2 * sizeof(int));
//         if (pipe(pipes[i]) == -1) {
//             perror("Error creating pipe");
//             exit(1);
//         }
//     }
    
//     // creamos hijos
//     for (i = 0; i < n; i++) {
//         if ((pid = fork()) == 0) {
//             int j;
//             int prev = (i == 0) ? n - 1 : i - 1;
//             int next = (i + 1) % n;
            
//             // cerramos los pipes que no necesitamos
//             for (j = 0; j < n; j++) {
//                 if (j != prev) close(pipes[j][1]);
//                 if (j != i) close(pipes[j][0]);
//             }
            
//             read(pipes[i][0], buffer, sizeof(int));
//             buffer[0]++;
//             write(pipes[next][1], buffer, sizeof(int));
            
//             // chau
//             close(pipes[i][0]);
//             close(pipes[prev][1]);
//             exit(0);
//         }
//     }
    
//     // padre
//     for (i = 0; i < n; i++) {
//         if (i != s - 1) close(pipes[i][0]);
//         if (i != ((s - 2 + n) % n)) close(pipes[i][1]);
//     }
    
//     // valor inicial al proceso que inicia
//     write(pipes[(s - 2 + n) % n][1], buffer, sizeof(int));
    
//     // valor final del último proceso
//     read(pipes[s - 1][0], buffer, sizeof(int));
    
//     close(pipes[(s - 2 + n) % n][1]);
//     close(pipes[s - 1][0]);
    
//     for (i = 0; i < n; i++) {
//         wait(&status);
//     }
    
//     for (i = 0; i < n; i++) {
//         free(pipes[i]);
//     }
//     free(pipes);
    
//     printf("Resultado final: %d\n", buffer[0]);
    
//     return 0;
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

    n = atoi(argv[1]);  // Número de procesos.
    c = atoi(argv[2]);  // Entero a transmitir.
    s = atoi(argv[3]);  // Proceso inicial.    
    
    if (n < 3) {
        printf("Error: <n> debe ser mayor igual a 3.\n");
        exit(1);
    }
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
        if ((pid = fork()) == 0) {  // si es el hijo
            int prev = (i == 0) ? n - 1 : i - 1;
            int next = (i + 1) % n;

            // cerrar pipes no usadaos
            for (int j = 0; j < n; j++) {
                if (j != prev) close(pipes[j][1]);
                if (j != i) close(pipes[j][0]);
            }

            // Si soy el proceso inicial, empiezo la comunicación
            if (i == s - 1) {
                c++;  // incremento el valor
                write(pipes[next][1], &c, sizeof(int));

            } else {
                // Los demás procesos esperan, reciben, incrementan y reenvían
                read(pipes[i][0], &c, sizeof(int));
                c++;
                write(pipes[next][1], &c, sizeof(int));
            }

            close(pipes[i][0]);
            close(pipes[prev][1]);
            exit(0);
        }
    } 
    
   // Padre: cerrar pipes no necesarios y esperar el resultado final
    for (int i = 0; i < n; i++) {
        if (i != s - 1) close(pipes[i][0]);  // Solo necesito leer del proceso inicial
        close(pipes[i][1]);  // No necesito escribir a ningún pipe
    }

    // Leer el resultado final del proceso inicial (que recibe el último valor)
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