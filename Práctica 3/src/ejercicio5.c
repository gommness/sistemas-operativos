/**
 * @brief prueba de la biblioteca de semaforos del ejercicio4.c
 *
 * @file ejercicio5.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */
#include "ejercicio4.h"

int semid; /*id de los semaforos*/

/*
debe ser global porque en el manejador de la sennal SIGINT necesitamos esta variable para eliminar los semaforos
que tenga asociados
 */

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/**
 * @brief manejador de la señal SIGINT que imprime un mensaje cuando se captura dicha señal y libera todos los recursos
 * @param sig la señal a manejar
 */
void manejador_SIGINT(int sig) {
    while (wait(NULL) != -1); /*de tener hijos, les espero*/
    Borrar_Semaforo(semid); /*elimino semaforos asociados*/
    exit(EXIT_FAILURE); /*terminacion del proceso*/
}

/**
 * @brief funcion que imprime el estado de los semaforos
 * @param semid el id de los semaforos
 * @param size el tamanno del array de semaforos
 */
int sem_printf(int semid, int size) {
    union semun arg;
    int i;
    fprintf(stdout, "\nestado de los semaforos:\n");
    for (i = 0; i < size; i++) {
        fprintf(stdout, "valor de sem[%d] = %d\n", i, semctl(semid, i, GETVAL, arg));
    }
    return 1;
}

int main(int argc, char* argv[]) {

    unsigned short sem[3]; /*!<el array de valores iniciales que utilizare para inicializar los semaforos*/
    int op[3]; /*!<el array con las operaciones que utilizare para hacer ups y downs multiples*/
    int pid; /*!<variable que guardara el retorno de fork()*/
    int i; /*!<iterador*/
    int flag; /*!<variable para deteccion de errores*/

    /*asigno el manejador, por si el usuario decide terminar los procesos con la sennal SIGINT*/
    if (signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }

    /*primero intento crear un array de 3 semaforos*/
    if (Crear_Semaforo(ftok("ejercicio5", getpid()), 3, &semid) == -1) {
        fprintf(stderr, "error al crear los semaforos");
        exit(EXIT_FAILURE);
    }

    /*introduzco los valores iniciales que quiero que tengan los semaforos en este array*/
    for (i = 0; i < 3; i++)
        sem[i] = 1;

    /*inicializo todos los semaforos (en este caso, a 1)*/
    if (Inicializar_Semaforo(semid, sem) != 0) {
        fprintf(stderr, "error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }

    /*creo un proceso hijo*/
    pid = fork();
    if (pid == -1) {
        fprintf(stderr, "error al hacer un fork");
        return -1;
    }

    /*en este bloque compruebo que up y down funcionan, bloqueando a un proceso de ser necesario*/
    fprintf(stdout, "\nel proceso %d va a hacer down del semaforo\n", getpid());
    Down_Semaforo(semid, 0, 1); /*comienza una region critica*/
    sem_printf(semid, 3);
    fprintf(stdout, "\nel proceso %d ha pasado por el semaforo\nprocedera a dormir 5 segundos\n", getpid());
    sleep(5);
    Up_Semaforo(semid, 0, 1); /*termina una region critica*/
    fprintf(stdout, "\nel proceso %d ha hecho up del semaforo\n", getpid());

    sleep(2);

    /*en este bloque comprobare que upMultiple y downMultiple funcionan correctamente*/

    /*primero, voy a reinicializar los valores de los semaforos*/
    for (i = 0; i < 3; i++)
        sem[i] = 0;
    if (Inicializar_Semaforo(semid, sem) != 0)/*reinicializo todos los semaforos a 0*/ {
        fprintf(stderr, "error al inicializar los semaforos");
        exit(EXIT_FAILURE);
    }

    /*aqui declaro sobre que semaforos actuaran las operaciones multiples*/
    op[0] = 0; /*no hare operacion sobre sem[0]*/
    op[1] = 1; /*hare operacion sobre sem[1]*/
    op[2] = 1; /*hare operacion sobre sem[2]*/

    if (pid != 0)/*codigo del padre*/ {
        sleep(2);
        fprintf(stdout, "\nPADRE: intento acceder a mi codigo\n");
        flag = Down_Semaforo(semid, 1, 1); /*region critica, espero a que el hijo este listo para comenzar*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }

        fprintf(stdout, "\nPADRE: hago up multiple (sem 1 y sem 2)\n");
        flag = UpMultiple_Semaforo(semid, 3, 1, op); /* "abro" la primera region critica. estado del semaforo: 0 1 1*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nPADRE: doy acceso al hijo\n");
        flag = Up_Semaforo(semid, 0, 1); /*indico a mi hijo que todo esta listo y puede proceder*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);

        fprintf(stdout, "\nPADRE: intento down(1)\n");
        /*primer semaforo de la primera region critica. Comprobamos si el up multiple funciona correctamente en sem[1]*/
        flag = Down_Semaforo(semid, 1, 1);
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nPADRE: conseguido, intento down(2)\n");
        /*segundo semaforo de la primera region critica. Comprobamos si el up multiple funciona correctamente en sem[2]*/
        flag = Down_Semaforo(semid, 2, 1);
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nPADRE: conseguido, accedo a la region critica protegida por dos semaforos\n\thago up multiple");
        flag = UpMultiple_Semaforo(semid, 3, 1, op); /*fin de la primera region critica*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nPADRE: intento down(1) y down(2)\n");
        flag = DownMultiple_Semaforo(semid, 3, 1, op); /*segunda region critica, esta vez se marcara con un down multiple*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nPADRE: conseguido. duermo 5 segs\n");
        sleep(5);
        flag = UpMultiple_Semaforo(semid, 3, 1, op); /*fin de la segunda region critica*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        fprintf(stdout, "\nPADRE: hago up multiple y termino mi parte sin problemas\n");
        Up_Semaforo(semid, 0, 1); /*indico a mi hijo que he terminado mi parte del codigo y que puede continuar*/
    } else if (pid == 0)/*codigo del hijo*/ {
        fprintf(stdout, "\nHIJO: doy acceso al padre\n");
        flag = Up_Semaforo(semid, 1, 1); /*indico a mi padre que ya he llegado a esta parte*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nHIJO: intento acceder a mi codigo\n");
        flag = Down_Semaforo(semid, 0, 1); /*espero a que mi padre me de acceso*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);

        fprintf(stdout, "\nHIJO: conseguido, intento down(1)\n");
        /*segundo semaforo de la primera region critica. Comprobamos si el up multiple funciona correctamente en sem[1]*/
        flag = Down_Semaforo(semid, 1, 1);
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nHIJO: conseguido, intento down(2)\n");
        /*segundo semaforo de la primera region critica. Comprobamos si el up multiple funciona correctamente en sem[2]*/
        flag = Down_Semaforo(semid, 2, 1);
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nHIJO: conseguido, accedo a la region critica protegida por dos semaforos\n\thago up multiple");
        flag = UpMultiple_Semaforo(semid, 3, 1, op); /*fin de la primera region critica*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nHIJO: intento down(1) y down(2)\n");
        flag = DownMultiple_Semaforo(semid, 3, 1, op); /*segunda region critica, esta vez se marcara con un down multiple*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        sleep(2);
        fprintf(stdout, "\nHIJO: conseguido. duermo 5 segs\n");
        sleep(5);
        flag = UpMultiple_Semaforo(semid, 3, 1, op); /*fin de la segunda region critica*/
        if (flag == -1) {
            Borrar_Semaforo(semid);
            while (wait(NULL) != -1);
            exit(1);
        }
        fprintf(stdout, "\nHIJO: hago up multiple y termino mi parte sin problemas\n");
        Down_Semaforo(semid, 0, 1); /*esperare a que mi padre termine su parte del codigo y me indique que puedo continuar*/
    }

    while (wait(NULL) != -1); /*espero a que todos los hijos terminen*/
    Borrar_Semaforo(semid); /*elimino el array de semaforos empleado*/

    return 0;
}


