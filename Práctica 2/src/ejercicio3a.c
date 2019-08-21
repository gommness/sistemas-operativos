/**
 * @brief Calcular el tiempo que se invierte en crear 100 procesos hijos
 * Cada hijo debe escribir por pantalla un numero aleatorio entre 0 y RAND_MAX
 *
 * @file ejercicio3a.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */
#include "types.h"
#define NUM_PROC 100

int main(int argc, char** argv) {
    int pid; 
    int time; /*!< tiempo que tarda la creacion de hijos */
    int i;
    struct timeval tv, tv2;
    gettimeofday(&tv, NULL); /*medimos el tiempo antes de crear los hijos*/
    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) < 0) {
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { /*entra aquí si es el proceso hijo*/
            srand(getpid());
            printf("Mi numero aleatorio es: %d\n", rand());
            exit(EXIT_SUCCESS);
        } else { /*entra aquí si es el proceso padre*/
            wait(NULL);
        }
    }
    gettimeofday(&tv2, NULL); /*medimos el tiempo al acabar*/
    time = (tv2.tv_sec - tv.tv_sec)*1000 + (tv2.tv_usec - tv.tv_usec) / 1000; /*calculamos el tiempo en milisegundos que tarda*/
    printf("%d milisegundos ha tardado en crear 100 hijos\n", time);
    return (EXIT_SUCCESS);
}

