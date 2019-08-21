/**
 * @brief Creacion de procesos hijo de forma secuencial
 *
 * @file ejercicio5a.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"

int main(void) {
    int pid;
    int inipid;
    int i;
    char PID[5];
    inipid = getpid();
    sprintf(PID, "%d", inipid); /*asi pasamos el pid del padre a string para ejecutar pstree*/
    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) < 0) {
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            printf("HIJO %d su padre es: %d\n", getpid(), getppid());
        } else {
            printf("PADRE %d\n", getpid());
            break; /*aqui se sale del bucle si se trata de un proceso padre, para que no pueda tener mas hijos*/
        }
    }
    wait(NULL);
    execlp("pstree", "pstree", "-p", PID, NULL); /*aqui podemos ver el estado del arbol de procesos*/
    exit(EXIT_SUCCESS);
}
