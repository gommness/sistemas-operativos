/**
 * @brief Cada proceso hijo imprime su pid y el de su proceso padre, pero hay un wait antes de finalizar el proceso, lo que evita la aparicion de procesos huérfanos.
 *
 * @file ejercicio4b.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"

int main(void) {
    int pid;
    int i;
    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) < 0) {
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { /*entra aquí si es el proceso hijo*/
            printf("pid HIJO %d pid PADRE %d\n", getpid(), getppid());
        } else { /*entra aquí si es el proceso padre*/
            printf("PADRE %d\n", getpid());
        }
    }
    wait(NULL); /*wait antes de acabar el proceso*/
    exit(EXIT_SUCCESS);
}
