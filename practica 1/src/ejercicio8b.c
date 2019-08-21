/**
 * @brief ejecutar en foreground o background un programa pasado por argumento dependiendo de si se pone b o no
 * @file ejercicio8b.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"

int main(int argc, char** argv) {

    int i;
    char cad[10];
    char *argsb[argc - 1]; /*<! se usa en caso de background*/
    char *argsf[argc]; /*!< se usa en caso de foreground*/
    
    if (argc < 2) {
        fprintf(stderr, "numero de argumentos incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(argv[argc - 1], "b") == 0) { /*aquí entra si añadimos una b al final del comando (para ejecutar en background)*/
        strcpy(cad,argv[1]);
        strcat(cad,"&"); /*aqui ira el primer argumento concatenado con un & para que se ejecute en background*/
        argsb[0] = cad; /*en argsvb guardaremos los argumentos a ejecutar, con cad como primer argumento y el resto de argumentos sin tener en cuenta la b*/
        for(i=1;i<argc-2;i++){ 
            argsb[i] = argv[i+1];
        }
        argsb[i] = NULL;
        execvp(argv[1],argsb); /*aqui ejecutamos los argumentos en background argsb*/
	exit(EXIT_SUCCESS);
    } else { /*aqui entra si se trata de foreground*/
        for(i=0;i<argc-1;i++){
            argsf[i] = argv[i+1];
        }
        argsf[argc-1] = NULL; /*copiamos los argumentos en otra variable para ejecutarlo*/
        execvp(argv[1],argsf);
    }

    return (EXIT_SUCCESS);
}

