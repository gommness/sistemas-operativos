/**
 * @brief Proceso que tiene 4 hijos en serie, donde se produce un intercambio de señales entre
 * padres e hijos
 *
 * @file ejercicio8.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */

#include "types.h"
#define NUM_PROC 4

/**
 * @brief manejador de la señal SIGUSR1 que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGUSR1(int sig) {
    printf("Mensaje SIGUSR1 enviado\n");
    fflush(stdout);
}
/**
 * @brief manejador de la señal SIGUSR2 que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGUSR2(int sig) {
    printf("Mensaje SIGUSR2 enviado\n");
    fflush(stdout);

}
/**
 * @brief manejador de la señal SIGTERM que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGTERM(int sig) {
    printf("Mensaje SIGTERM enviado\n");
    fflush(stdout);
}

int main(int argc, char** argv) {
    void manejador_SIGUSR1();
    void manejador_SIGUSR2();
    void manejador_SIGTERM();
    int pid, ppid; /*!< pid del hijo correspondiente y pid del padre raíz */
    int i;
    
    ppid = getpid(); /*pid del padre raiz*/

    /*Armamos las diferentes señales*/
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura1");
        exit(1);
    }
    if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }
    if (signal(SIGTERM, manejador_SIGTERM) == SIG_ERR) {
        puts("Error en la captura3");
        exit(1);
    }

    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) < 0) {
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);

        } else if (pid == 0) {
            if (i == 3) { /*codigo correspondiente al hijo último*/
                printf("Aqui empieza el primer envío de SIGUSR1\n");
                fflush(stdout);
                sleep(5);
                kill(getppid(), SIGUSR1); /*manda señal como hijo a padre*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSR2\n", getpid());
                fflush(stdout);
                kill(ppid, SIGUSR2); /*envia señal a la raiz cuando le mandan una señal*/
                pause();
                printf("proceso %d ha recibido la señal SIGTERM\n", getpid());
                fflush(stdout);
                sleep(1);
                kill(ppid, SIGTERM);
            }
        } else {
            if (i > 0) { /*codigo correspondiente a los padres no raiz*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSR1\n", getpid());
                fflush(stdout);
                sleep(2);
                kill(getppid(), SIGUSR1); /*envia esto a su padre*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSR2\n", getpid());
                fflush(stdout);
                sleep(1);
                kill(pid, SIGUSR2); /*envia esto a su hijo*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSRTERM\n", getpid());
                fflush(stdout);
                sleep(1);
                kill(pid, SIGTERM);
            } else if (i == 0) {/*codigo correspondiente al padre raiz*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSR1\n", getpid());
                printf("Aqui empieza el primer envío de SIGUSR2\n");
                fflush(stdout);
                kill(pid, SIGUSR2); /*envia señal a su hijo*/
                pause();
                printf("proceso %d ha recibido la señal SIGUSR2\n", getpid());
                fflush(stdout);
                sleep(1);
                printf("Aqui empieza el primer envío de SIGTERM\n");
                kill(pid, SIGTERM);
                pause();
                printf("proceso %d ha recibido la señal SIGTERM\n", getpid());
                fflush(stdout);
            }

            break; /*aqui se sale del bucle si se trata de un proceso padre, para que no pueda tener mas hijos*/
        }
    }
    wait(NULL);
    return (EXIT_SUCCESS);
}

