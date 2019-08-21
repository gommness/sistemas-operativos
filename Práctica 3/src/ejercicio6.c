/**
 * @brief problema de sincronizacion entre procesos que quieren acceder a un mismo recurso con ciertas restricciones de sincronización
 *
 * @file ejercicio6.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */
#include "ejercicio4.h"
#define SEMS 4/*macro que contiene el tamanno del array de semaforos*/

int semid; /*id de los semaforos*/
int id_zone; /*id de la memoria compartida*/

/*
deben ser globales porque en el manejador de la sennal SIGINT necesitamos estas variables para eliminar los recursos
que tengan asociados
 */

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct info {
    int van; /*contador de coches correspondiente al sentido 0, VAN*/
    int vienen; /*contador de coches correspondiente al sentido 1, VIENEN*/
    int flag; /*bandera de salida*/
};

/**
 * @brief funcion que imprime el estado de un semaforo
 * @param semid el id de los semaforos
 * @param num el numero del semaforo dentro del id
 */
int sem_printf(int semid, int num) {
    union semun arg;
    fprintf(stdout, "sem[%d] = %d ", num, semctl(semid, num, GETVAL, arg));
    fprintf(stdout, "\n");
    fflush(stdout);
    return 1;
}

/**
 * @brief manejador de la señal SIGINT que imprime un mensaje cuando se captura dicha señal y libera todos los recursos
 * @param sig la señal a manejar
 */
void manejador_SIGINT(int sig) {
    while (wait(NULL) != -1); /*si el proceso tiene hijos, les espera*/
    Borrar_Semaforo(semid); /*elimina los semaforos*/
    shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL); /*elimina memoria compartida*/
    exit(EXIT_FAILURE); /*finalizacion del proceso*/
}

int main(int argc, char* argv[]) {
    void manejador_SIGINT(); /*!< el manejador de la sennal SIGINT*/
    unsigned short sem[SEMS]; /*!< el array de valores iniciales que utilizare para inicializar los semaforos*/
    int pid; /*!< aqui guardare el retorno de fork*/
    int sentido; /*!< aqui guardare el sentido en el que un coche atraviesa el puente*/
    int i; /*!< iterador*/
    struct info *buffer; /*!< estructura con la memoria compartida*/
    key_t key; /*!< aqui guardo la clave para obtener memoria compartida y el array de semaforos*/

    /*asigno el manejador para la sennal SIGINT*/
    if (signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }

    /*creo mi array de semaforos*/
    key = ftok("ejercicio6", 2700);
    if (Crear_Semaforo(key, SEMS, &semid) == -1) {
        fprintf(stdout, "error al crear semaforos\n");
        exit(EXIT_FAILURE);
    }

    sem[0] = 0; /*el 0 sera un flag para que los hijos esperen a que el padre les de la sennal*/
    sem[1] = 1; /*el 1 servira para proteger region critica de entrada en sentido 0 (VAN) y de espera hasta que el padre de el pistoletazo de salida*/
    sem[2] = 1; /*el 2 sera el semaforo para proteger el sentido del puente*/
    sem[3] = 1; /*el 3 servira para proteger la region critica de entrada en sentido 1 (VIENEN)*/

    if (Inicializar_Semaforo(semid, sem) != 0) {
        fprintf(stderr, "error al inicializar los semaforos");
        kill(getpid(), SIGINT);
    }
    /*obtengo el id de la zona de memoria que sera compartida*/
    id_zone = shmget(key, sizeof (struct info)*1, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone == -1) {
        fprintf(stderr, "error al obtener memoria compartida");
        kill(getpid(), SIGINT);
    }

    /*engancho a todos memoria compartida*/
    buffer = shmat(id_zone, (struct info *) 0, 0);
    buffer->flag = 0; /*la bandera de salida esta bajada*/
    buffer->van = 0; /*en el puente no hay coches que VAN*/
    buffer->vienen = 0; /*en el puente no hay coches que VIENEN*/


    /*creacion de los 100 procesos, 50 en un sentido y 50 en otro sentido*/
    for (i = 0; i < 100; i++) {
        pid = fork();
        if (pid == -1) {
            fprintf(stdout, "error al hacer el fork\n");
            kill(getpid(), SIGINT);
        } else if (pid == 0) {
            /*asignamos un sentido al proceso. o bien 0 o bien 1, de forma alternada*/
            sentido = i % 2;
            break;
        }
    }

    if (pid != 0) {/*codigo del padre*/
        fprintf(stdout, "\nPADRE: termino de crear los coches. Comienza la cuenta atras (3 segs)\n");
        sleep(3); /*espero 3 segundos antes de dar la salida*/
        buffer->flag = 1; /*levanto la bandera de salida*/
        Up_Semaforo(semid, 0, 1); /*indico a mis hijos que ya he terminado de crearles a todos. pueden comenzar*/
        while (wait(NULL) != -1); /*esperare a que todos mis hijos terminen*/
        /*cuando todos los procesos hijos hayan terminado, procedo a liberar recursos*/
        Borrar_Semaforo(semid); /*libero los semaforos*/
        shmdt((char*) buffer); /*me desengancho de la memoria compartida*/
        shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL); /*libero el bloque de la memoria compartida*/
    } else if (pid == 0) {/*codigo de los hijos*/

        Down_Semaforo(semid, 1, 1); /*region critica*/
        if (buffer->flag == 0) {/*en caso de que el padre no haya levantado aun la bandera de salida*/
            Down_Semaforo(semid, 0, 1); /*se bloquea aqui hasta que el padre le indique (cuando cree todos los 100 procesos)*/
            printf("\ncomienza el arranque de todos los coches\n\n");
        }
        Up_Semaforo(semid, 1, 1); /*fin de la region critica*/


        /*a partir de aqui empieza el problema del puente*/

        srand(getpid()); /*obtengo semilla random a partir del pid del proceso*/
        sleep(rand() % 20); /*duermo un numero aleatorio de segundos (entre 0 y 19)*/

        if (sentido == 0) {/*procesos que van en sentido 0*/

            Down_Semaforo(semid, 1, 1); /*region critica para entrar al puente*/
            ++buffer->van;
            if (buffer->van == 1) {/*en caso de ser el primer coche que va*/
                /*
                LIGHT SWITCH ON
                hago down del semaforo del puente:
                    -si no hubiera nadie cruzando en el sentido opuesto, indico que ahora hay alguien cruzando en mi sentido.
                        por tanto todo coche que a partir de ahora venga en sentido opuesto se bloqueara
                    -si hay alguien cruzando en sentido opuesto, me bloqueo hasta que me indiquen que ya no quedan coches cruzando en sentido
                        opuesto
                 */
                Down_Semaforo(semid, 2, 0);
            }
            printf("entra coche %d VAN %d\n", getpid(), buffer->van); /*entro definitivamente en el puente*/
            Up_Semaforo(semid, 1, 1); /*fin de region critica para entrar en el puente*/

            sleep(1); /*cruzo el puente*/

            Down_Semaforo(semid, 1, 1); /*region critica para salir del puente*/
            --buffer->van;
            printf("sale coche %d VAN %d\n", getpid(), buffer->van);
            if (buffer->van == 0) {/*en caso de que ya no queden coches cruzando en mi sentido*/
                /*
                LIGHT SWITCH OFF
                hago up del semaforo del puente:
                    -si habia alguien esperando a cruzar en sentido opuesto, podra proceder a cruzar
                    -si no habia nadie en sentido opuesto, se indica que ahora podran llegar de cualquier sentido
                 */
                Up_Semaforo(semid, 2, 0);
            }
            Up_Semaforo(semid, 1, 1); /*fin de la region critica para salir del puente*/

        } else if (sentido == 1) {/*procesos que vienen en sentido 1*/

            Down_Semaforo(semid, 3, 1); /*region critica para entrar en el puente*/
            ++buffer->vienen;
            if (buffer->vienen == 1) {
                /*
                LIGHT SWITCH ON
                hago down del semaforo del puente:
                    -si no hubiera nadie cruzando en el sentido opuesto, indico que ahora hay alguien cruzando en mi sentido.
                        por tanto todo coche que a partir de ahora venga en sentido opuesto se bloqueara
                    -si hay alguien cruzando en sentido opuesto, me bloqueo hasta que me indiquen que ya no quedan coches cruzando en sentido
                        opuesto
                 */
                Down_Semaforo(semid, 2, 0);
            }
            printf("entra coche %d VIENEN %d\n", getpid(), buffer->vienen); /*entro definitivamente al puente*/
            Up_Semaforo(semid, 3, 1); /*fin de la region critica para entrar al puente*/

            sleep(1);

            Down_Semaforo(semid, 3, 1); /*region critica para salir del puente*/
            --buffer->vienen;
            printf("sale coche %d VIENEN %d\n", getpid(), buffer->vienen);
            if (buffer->vienen == 0) {
                /*
                LIGHT SWITCH OFF
                hago up del semaforo del puente:
                    -si habia alguien esperando a cruzar en sentido opuesto, podra proceder a cruzar
                    -si no habia nadie en sentido opuesto, se indica que ahora podran llegar de cualquier sentido
                 */
                Up_Semaforo(semid, 2, 0);
            }
            Up_Semaforo(semid, 3, 1); /*fin de la region critica para salir del puente*/

        }
        shmdt((char*) buffer); /*los hijos se desenganchan la memoria compartida*/
    }
    exit(0);
}
