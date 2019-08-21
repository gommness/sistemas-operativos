/**
 * @brief Un proceso padre crea a su hijo, al cual, pasado 30 unidades de tiempo
 * procede a terminarlo.
 *
 * @file ejercicio6.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */
#include "types.h"

int main(int argc, char** argv) {
    int pid;
    if ((pid = fork()) < 0) {
        printf("Error al emplear fork\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { /*entra aquí si es el proceso hijo*/
        while (1) {
            printf("Soy el proceso hijo con PID: %d\n", getpid());
            sleep(5);
        }
    } else { /*entra aquí si es el proceso padre*/
        sleep(30);
        /*Manda la señal de terminación al hijo*/
        if (kill(pid, SIGKILL) == 0) {
            wait(NULL);
        } else {
            return EXIT_FAILURE;
        }
    }
    return (EXIT_SUCCESS);
}
