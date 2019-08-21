/**
 * @brief Creacion de procesos hijo todos de un mismo padre
 *
 * @file ejercicio5b.c
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
    sprintf(PID, "%d", inipid); /*obtenemos el PID del padre como string*/
    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) < 0) {
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            printf("HIJO %d su padre es: %d\n", getpid(), getppid());
            execlp("pstree", "pstree", "-p", PID, NULL); /*si se trata del proceso hijo, realiza este comando y termina su ejecucion*/
        } else {
            printf("PADRE %d\n", getpid()); /*si se trata del padre sigue realizando forks en el bucle*/
        }
    }
    while (wait(NULL) != -1);

    exit(EXIT_SUCCESS);
}
