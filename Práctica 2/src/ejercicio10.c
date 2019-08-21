/**
 * @brief El proceso B crea al proceso A.
 * El proceso A se dedica a imprimir por pantalla una palabra del buffer aleatoriamente,
 * si imprime FIN se termina
 * El proceso B se encarga de leer lo que escribe el proceso A, si se termina el A,
 * lo vuelve a crear.
 *
 * @file ejercicio10.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */

#include "types.h"
#define ITER 50

int main(int argc, char** argv) {
    int pid, i;
    FILE *f_lect, *f_write; /*!< mismo fichero para leer y para escribir*/
    char temp[10], *buffer[13] = {"EL", "PROCESO", "A", "ESCRIBE", "EN", "UN",
        "FICHERO", "HASTA", "QUE", "LEE", "LA", "CADENA", "FIN"};

    pid = fork();
    for (i = 0; i < ITER; i++) {
        if ((pid) < 0) {
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);

        } else if (pid == 0) {/*proceso A escribe*/
            srand(getpid());
            while (1) {
                f_write = fopen("fichero.txt", "w");
                strcpy(temp, buffer[rand() % 13]);
                fprintf(f_write, "%s", temp); /*imprime una palabra aleatoria de la frase*/
                fclose(f_write);
                if (strcmp(temp, "FIN") == 0) { /*si escribe FIN, se termina*/
                    exit(0);
                }
                sleep(4); /*le colocamos un retardo a la escritura*/
            }

        } else {/*proceso B lee*/
            sleep(5);
            f_lect = fopen("fichero.txt", "r");
            fscanf(f_lect, "%s", temp);
            printf("He leido %s\n", temp);
            if (strcmp(temp, "FIN") == 0) { /*si lee FIN, es que el hijo ha acabado,
                                             asi que lo vuelve a crear*/
                wait(NULL);
                printf("Se ha creado otro proceso\n");
                pid = fork();
            }
        }
    }
    fclose(f_lect);
    kill(pid, SIGKILL);
    wait(NULL);
    return (EXIT_SUCCESS);
}

