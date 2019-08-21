/**
 * @brief Pasar 2 numeros y 2 matrices cuadradas desde linea de comandos y
 * Hacer la multiplicacion de los numeros con sus respectivas matrices mediante hilos
 *
 * @file ejercicio4.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 04-03-2016
 */
#include "types.h"

struct arg_struct {
    int tam; /*!< tamaño de la matriz*/
    int id; /*!< identificador de hilo*/
    int mult; /*!< primer multiplicador*/
    int **matrix; /*!< matriz a multiplicarle el primer multiplicador*/
    int fila; /*!< fila en la que se encuentra operando*/
    int *filavec; /*!< num fila por la que va de la otra matriz multiplicando*/
    int *idvec; /*!< id del otro hilo*/
};

/**
* @brief multiplica una matriz y un numero que se pasa por argumento.
*
* @param arguments una estructura con los argumentos necesarios para multiplicar
 * una matriz con un numero, esta estructura viene especificada arriba.
* @return nada
*/
void* multiplica_matriz(void* arguments) {
    int j;
    struct arg_struct *args = arguments;
    for (args->fila = 0; args->fila < args->tam; args->fila++) { /*recorro las filas de la matriz para multiplicar los elementos*/
        printf("Hilo %d multiplicando fila %d resultado ", args->id, args->fila);
        
        for (j = 0; j < args->tam; j++) {
            printf("%d ", args->mult * args->matrix[args->fila][j]);
        }
        printf("– el Hilo %d va por la fila %d\n", *args->idvec, *args->filavec);

        fflush(stdout);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    int i, j, a = 3, b = 6, tam, **M, **M2;
    pthread_t h1, h2;
    struct arg_struct args, args2;
    char* buffer, *token;

    printf("Introduzca multiplicador 1:\n");
    scanf("%d", &a);
    printf("Introduzca multiplicador 2:\n");
    scanf("%d", &b);
    printf("Introduzca tamaño de matrices:\n");
    scanf("%d", &tam);
    M = (int**) malloc(tam * sizeof (int*));
    if (M == NULL) {
        fprintf(stderr, "Error al reservar memoria para la matriz 1");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < tam; i++) {
        M[i] = (int*) malloc(tam * sizeof (int));
        if (M[i] == NULL) {
            fprintf(stderr, "Error al reservar memoria para la matriz 1");
            exit(EXIT_FAILURE);
        }
    }
    M2 = (int**) malloc(tam * sizeof (int*));
    if (M2 == NULL) {
        fprintf(stderr, "Error al reservar memoria para la matriz 2");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < tam; i++) {
        M2[i] = (int*) malloc(tam * sizeof (int));
        if (M2[i] == NULL) {
            fprintf(stderr, "Error al reservar memoria para la matriz 2");
            exit(EXIT_FAILURE);
        }
    }


    printf("Introduzca matriz 1:\n");
    buffer = (char*) malloc((2 * tam * tam + 1) * sizeof (char));
    getchar();
    fgets(buffer, 2 * tam*tam, stdin);
    token = strtok(buffer, " ");
    for (i = 0; i < tam; i++) {
        for (j = 0; j < tam; j++) {
            M[i][j] = atoi(token);
            token = strtok(NULL, " ");
        }
    }
    printf("Introduzca matriz 2:\n");
    getchar();
    fgets(buffer, 2 * tam*tam, stdin);
    token = strtok(buffer, " ");
    for (i = 0; i < tam; i++) {
        for (j = 0; j < tam; j++) {
            M2[i][j] = atoi(token);
            token = strtok(NULL, " ");
        }
    }
    printf("Realizando producto:\n");
    /*Aquí nos dedicamos a asignar los valores a las estructuras*/
    args.tam = args2.tam = tam;
    args.id = 1;
    args2.id = 2;
    args.mult = a;
    args2.mult = b;
    args.matrix = M;
    args2.matrix = M2;
    args.fila = args2.fila = 0;
    args.filavec = &args2.fila;
    args2.filavec = &args.fila;
    args.idvec = &args2.id;
    args2.idvec = &args.id;
    /*creamos los hilos para que hagan las multiplicaciones*/
    pthread_create(&h1, NULL, multiplica_matriz, (void*) &args);
    pthread_create(&h2, NULL, multiplica_matriz, (void*) &args2);
    pthread_join(h1, NULL);
    pthread_join(h2, NULL);
    
    /*Liberamos la memoria reservada*/
    free(buffer);
    for (i = 0; i < tam; i++) {
        free(M[i]);
        free(M2[i]);
    }
    free(M);
    free(M2);
    return (EXIT_SUCCESS);
}