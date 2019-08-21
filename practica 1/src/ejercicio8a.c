/**
 * @brief Imprime el tamaño del ejecutable asociado al programa por terminal
 *
 * @file ejercicio8a.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"

int main(int argc, char** argv) {
    execlp("du", "du","-b","-BK", "ejercicio8a", NULL);	/*utilizamos el comando du junto a -BK para que imprima en KB el tamaño del ejecutable*/
    exit(EXIT_SUCCESS);
}

