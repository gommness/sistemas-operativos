/**
 * @brief En este ejercicio simulamos el envío de mensajes entre procesos padre e hijos
 *
 * @file cadena_montaje.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 21-04-2016
 */

#include "types.h"
#define NUM_PROC 2
#define N 27

typedef struct _Mensaje {
    long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
    int tamano; /*tamaño del mensaje*/
    char aviso[SIZE]; /*el mensaje*/
} mensaje;
int msqid;

/**
 * @brief manejador de la señal SIGINT que imprime un mensaje cuando se captura dicha señal y libera todos los recursos
 * @param sig la señal a manejar
 */
void manejador_SIGINT(int sig) {
    while (wait(NULL) != -1); /*si el proceso tiene hijos, les espera*/
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
    exit(EXIT_FAILURE); /*finalizacion del proceso*/
}

/**
 * @brief manejador de la señal SIGUSR1 que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGUSR1(int sig) {
    fflush(stdout);
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }
}

/**
 * @brief Esta funcion devuelve el numero de mensajes pendientes en la cola en ese momento
 * @param sig la señal a manejar
 * @return el numero de mensajes en la cola
 */
int msgPend(int msqid) {
    struct msqid_ds buf;
    msgctl(msqid, IPC_STAT, &buf);
    return buf.msg_qnum;
}

int main(int argc, char** argv) {
    FILE* f1, *f2;
    int pid, i, contador, tamanorestante;
    key_t clave;
    mensaje msg;
    char* buffer;


    if (argc != 3) {
        perror("Numero de argumentos incorrecto\n");
        return ERROR;
    }
    /*asigno el manejador para la sennal SIGINT*/
    if (signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }
    /*asigno el manejador para la sennal SIGINT*/
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }

    f1 = fopen(argv[1], "r");
    f2 = fopen(argv[2], "w");

    clave = ftok("cadena_montaje", N);
    if (clave == (key_t) - 1) {
        perror("Error al obtener clave para cola mensajes\n");
        exit(EXIT_FAILURE);
    }

    msqid = msgget(clave, 0600 | IPC_CREAT);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes");
        return (0);
    }

    /*Creo hijos de forma secuencial*/
    for (i = 0; i < NUM_PROC; i++) {
        if ((pid = fork()) == -1) {
            perror("Error en el fork\n");
            return ERROR;
        } else if (pid) {
            break;
        }
    }
    /*proceso A*/
    if (i == 2) {
        /*Las primeras lineas del fichero se leen normal con fread de tamaño SIZE*/

        msg.id = 1; /*Tipo de mensaje*/
        msg.tamano = SIZE;
        while (!feof(f1)) {
            buffer = (char*) malloc(SIZE * sizeof (char));
            fread(buffer, SIZE, 1, f1);
            strcpy(msg.aviso, buffer);
            if (!feof(f1)) {
                /*Se envia el mensaje a menos que haya llegado al ultimo cacho del fichero*/
                if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
                    printf("ERROR al enviar\n");
                }
            }
            free(buffer);
        }
        /*Calculo el tamano del ultimo cacho del fichero que ocupa menos de 4KB*/
        tamanorestante = (ftell(f1) + 1) % 4096;
        buffer = (char*) malloc(tamanorestante * sizeof (char));
        /*Me coloco donde empieza el ultimo cacho y me pongo a leer*/
        fseek(f1, -tamanorestante, SEEK_END);

        fread(buffer, tamanorestante, 1, f1);
        msg.tamano = tamanorestante;
        strcpy(msg.aviso, buffer);
        if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
            printf("ERROR al enviar\n");
        }
        free(buffer);
        /*Mando una señal al proceso B para avisar de que ha terminado de leer mensajes*/
        kill(getppid(), SIGUSR1);

    } else if (i == 1) {
        pause();
        /*Hasta que no ha terminado de enviar el otro proceso, no empieza a hacer cosas*/
        contador = msgPend(msqid); /*Calcula el numero de mensajes en la cola*/
        for (; contador > 0; contador--) {
            if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 1, 0) == -1)
                printf("ERROR al recibir\n");
            fflush(stdout);
            /*Pasa a mayusculas los caracteres*/
            for (i = 0; i < strlen(msg.aviso); i++) {
                if (msg.aviso[i] < 123 && msg.aviso[i] > 96) {
                    msg.aviso[i] -= 32;
                }
            }
            fflush(stdout);
            if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
                printf("ERROR al enviar\n");
            }

        }
        /*Proceso C*/
    } else if (i == 0) {
        /*Hasta que no han terminado sus hijos no empieza a actuar*/
        while (wait(NULL) != -1);
        contador = msgPend(msqid);
        for (; contador > 0; contador--) {
            if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 1, 0) == -1)
                printf("ERROR al recibir\n");
            fwrite(msg.aviso, msg.tamano, 1, f2);
        }

        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
    }
    return (EXIT_SUCCESS);
}




