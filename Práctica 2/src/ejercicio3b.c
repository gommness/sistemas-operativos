/**
 * @brief Calcular el tiempo que se invierte en crear 100 hilos
 * Cada hilo debe escribir por pantalla un numero aleatorio entre 0 y RAND_MAX
 *
 * @file ejercicio3b.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */

#include "types.h"
#define NUM_THRD 100
/**
 * @brief imprime un numero aleatorio, cuando es llamado por pthread_create()
 *
 */
void *printrand() {
    srand(pthread_self());
    printf("Mi numero aleatorio es: %d\n", rand());
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int time; /*!< tiempo que tarda la creacion de hijos */
    int i;
    struct timeval tv, tv2;
    pthread_t h[100];
    gettimeofday(&tv, NULL); /*medimos el tiempo antes de crear los hijos*/
    for (i = 0; i < NUM_THRD; i++) {
        pthread_create(&h[i], NULL, printrand, NULL);
    }
    for (i = 0; i < NUM_THRD; i++) {
        pthread_join(h[i], NULL);
    }
    gettimeofday(&tv2, NULL); /*medimos el tiempo al acabar*/
    time = (tv2.tv_sec - tv.tv_sec)*1000 + (tv2.tv_usec - tv.tv_usec) / 1000; /*calculamos el tiempo en milisegundos que tarda*/
    printf("%d milisegundos ha tardado en crear 100 hilos\n", time);
    return (EXIT_SUCCESS);
}