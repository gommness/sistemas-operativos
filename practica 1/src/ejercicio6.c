/**
 * @brief Este ejercicio nos permite comprobar como actuan los procesos creados por fork ante reservas de memoria dinamicas y la accesibilidad de los procesos a las variables de su otro proceso hijo/padre
 *
 * @file ejercicio6.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"

int main(int argc, char** argv) {
    char * cad;
    int pid;

    cad = (char*) malloc(20 * sizeof (char));

    if((pid = fork()) < 0){
        printf("Error al emplear fork\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { /*proceso hijo*/
        printf("introduzca una cadena de 20 caracteres: ");
        scanf("%s", cad);
        printf("el proceso hijo tiene acceso al valor: %s\n", cad); /*imprime el valor*/
    } else { /*proceso padre*/
        wait(NULL);
        printf("el proceso padre tiene acceso al valor: %s\n", cad); /*trata de imprimir el valor de su hijo, pero no tiene acceso a la variable cad*/
    }
    free(cad); /*hay que liberar la memoria en ambos procesos*/

    return (EXIT_SUCCESS);
}

