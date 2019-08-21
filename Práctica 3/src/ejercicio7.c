/**
 * @brief problema analogo al ejercicio6.c cuya implementacion ahora es a nivell de hilos
 *
 * @file ejercicio7.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */
#include "ejercicio4.h"

#define SEMS 4/*macro que contiene el tamanno del array de semaforos*/

int van; /*contador de coches que van*/
int vienen; /*contador de coches que vienen*/
int flag; /*bandera de salida*/

/*declaro los semaforos para hilos*/
pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER; /*servira para controlar la bandera de salida*/
pthread_mutex_t mutex_van = PTHREAD_MUTEX_INITIALIZER; /*servira para controlar la zona critica de los coches que van*/
pthread_mutex_t mutex_vienen = PTHREAD_MUTEX_INITIALIZER; /*servira para controlar la zona critica de los coches que vienen*/
pthread_mutex_t mutex_puente = PTHREAD_MUTEX_INITIALIZER; /*servira para controlar la zona critica de la direccion del puente*/

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void* cruza_puente(void* arg) {

    int sentido;
    char* dir;
    int* cont;
    pthread_mutex_t * mutex;

    pthread_mutex_lock(&mutex_van); /*region critica*/
    if (flag == 0) {/*en caso de que el padre no haya levantado aun la bandera de salida*/
        pthread_mutex_lock(&mutex_flag); /*se bloquea aqui hasta que el padre le indique (cuando cree todos los 100 procesos)*/
        printf("\ncomienza el arranque de todos los coches\n\n");
    }
    pthread_mutex_unlock(&mutex_van); /*fin de la region critica*/

    /*a partir de aqui empieza el problema del puente*/
    srand(pthread_self()); /*obtengo semilla random a partir del pid del proceso*/
    sleep(rand() % 20); /*duermo un numero aleatorio de segundos (entre 0 y 19)*/

    sentido = *((int*) arg);
    if (sentido == 0) {
        dir = "VAN";
        cont = &van;
        mutex = &mutex_van;
    } else {
        dir = "VIENEN";
        cont = &vienen;
        mutex = &mutex_vienen;
    }

    pthread_mutex_lock(mutex); /*region critica para entrar al puente*/
    ++(*cont);
    if ((*cont) == 1) {/*en caso de ser el primer coche que va*/
        /*
        LIGHT SWITCH ON
        hago down del semaforo del puente:
            -si no hubiera nadie cruzando en el sentido opuesto, indico que ahora hay alguien cruzando en mi sentido.
                por tanto todo coche que a partir de ahora venga en sentido opuesto se bloqueara
            -si hay alguien cruzando en sentido opuesto, me bloqueo hasta que me indiquen que ya no quedan coches cruzando en sentido
                opuesto
         */
        pthread_mutex_lock(&mutex_puente);
    }
    printf("entra coche %d %s %d\n", (int) pthread_self(), dir, (*cont)); /*entro definitivamente en el puente*/
    pthread_mutex_unlock(mutex); /*fin de region critica para entrar en el puente*/

    sleep(1); /*cruzo el puente*/

    pthread_mutex_lock(mutex); /*region critica para salir del puente*/
    --(*cont);
    printf("sale coche %d %s %d\n", (int) pthread_self(), dir, (*cont));
    if ((*cont) == 0) {/*en caso de que ya no queden coches cruzando en mi sentido*/
        /*
        LIGHT SWITCH OFF
        hago up del semaforo del puente:
            -si habia alguien esperando a cruzar en sentido opuesto, podra proceder a cruzar
            -si no habia nadie en sentido opuesto, se indica que ahora podran llegar de cualquier sentido
         */
        pthread_mutex_unlock(&mutex_puente);
    }
    pthread_mutex_unlock(mutex); /*fin de la region critica para salir del puente*/
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    void manejador_SIGINT(); /*!< el manejador de la sennal SIGINT*/
    int i; /*!< iterador*/
    int sentido[100];
    pthread_t h[100]; /*array de hilos*/

    pthread_mutex_lock(&mutex_flag);
    /*inicializo la memoria compartida, en este caso, variables globales*/
    van = 0;
    vienen = 0;
    flag = 0;

    /*CREACION DE LOS 100 HILOS*/
    for (i = 0; i < 100; i++) {
        sentido[i] = i % 2;
        pthread_create(&h[i], NULL, cruza_puente, (void*) (&sentido[i]));
    }
    fprintf(stdout, "\nHILO PRINCIPAL: termino de crear los coches. Comienza la cuenta atras (3 segs)\n");
    sleep(3); /*espero 3 segundos antes de dar la salida*/
    flag = 1; /*levanto la bandera de salida*/
    pthread_mutex_unlock(&mutex_flag); /*indico a los coches que ya he terminado de crearles a todos. pueden comenzar*/
    for (i = 0; i < 100; i++) {/*espero a la finalizacion de todos los hilos*/
        pthread_join(h[i], NULL);
    }
    exit(0);
}

