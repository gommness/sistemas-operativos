/**
 * @brief Crear memoria compartida, acceder y modificarla desde procesos distintos
 *
 * @file ejercicio2.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */

#include "includes.h"

int id_zone;

struct info {
    char nombre[80];
    int id;
};

/**
 * @brief manejador de la señal SIGINT que imprime un mensaje cuando se captura dicha señal y libera todos los recursos
 * @param sig la señal a manejar
 */
void manejador_SIGINT(int sig) {
    printf("sennal SIGINT recibida\n");
    fflush(stdout);
    while (wait(NULL) != -1);
    shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
    exit(EXIT_FAILURE);
}

/**
 * @brief manejador de la señal SIGUSR1 que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGUSR1(int sig) {
    printf("sennal SIGUSR1 recibida\n");
    fflush(stdout);
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura1");
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    void manejador_SIGUSR1(); /*!< manejador de la sennal SIGUSR1*/
    void manejador_SIGINT(); /*!< manejador de la sennal SIGINT*/
    int pid; /*!< almacenara el retorno de fork*/
    int n_proc; /*!< almacenara el numero de procesos que haya que crear*/
    int i; /*!< iterador*/
    int key; /*!< almacena la clave para reservar memoria compartida*/
    struct info* buffer; /*!< estructura con los campos deseados para la memoria compartida*/

    /*compruebo que efectivamente se ha introducido un numero de procesos*/
    if (argc != 2) {
        fprintf(stderr, "numero de argumentos incorrecto\nformato:\t./ejercicio2 <<n_proc>>\n\t\tsiendo n_proc el numero de procesos deseado");
        exit(1);
    }

    /*asigno el manejador para la sennal SIGUSR1*/
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura1");
        exit(1);
    }
    /*asigno el manejador para la sennal SIGINT*/
    if (signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }

    /*creo una clave para la memoria compartida*/
    key = ftok("ejercicio2", 2700);
    if (key == -1) {
        fprintf(stderr, "error con la clave");
        exit(-1);
    }

    /*obtengo el id de la zona de memoria que sera compartida*/
    id_zone = shmget(key, sizeof (struct info)*1, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone == -1) {
        id_zone = shmget(key, sizeof (struct info)*1, SHM_R | SHM_W);
        if(id_zone == -1){
            fprintf(stderr, "error al obtener memoria compartida");
            exit(-1);
        }
    }

    /*creo tantos procesos hijos como se haya especificado a la hora de introducir el programa*/
    n_proc = atoi(argv[1]);
    for (i = 0; i < n_proc; i++) {
        pid = fork();
        if (pid == -1) {
            fprintf(stderr, "numero de argumentos incorrecto");
            shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
            exit(1);
        } else if (pid == 0) /*si se trata del hijo, sale del bucle. Nos interesa que solo haya un padre con n_proc hijos*/
            break;
    }

    /*CODIGO COMUN PARA ENGANCHAR LA MEMORIA COMPARTIDA*/
    buffer = shmat(id_zone, (struct info *) 0, 0);
    if (buffer == NULL) {
        fprintf(stderr, "error al enganchar la memoria compartida");
        if (pid != 0) {
            while (wait(NULL) != -1); /*si el padre tiene este error, espera a todos sus hijos*/
        }
        shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
        exit(-1);
    }

    if (pid == 0) /*codigo de los hijos*/ {
        /*obtengo una semilla random basada en el id del proceso*/
        srand(getpid());
        /*duermo un tiempo aleatorio de segundos*/
        sleep((int) (rand() % 20));
        fprintf(stdout, "por favor, introduzca el nombre de un cliente:");
        /*puede haber problemas de concurrencia a la hora de escribir en las variables compartidas
        debido a que en el enunciado no se especifica nada de la zona critica*/
        /*leo el nombre itroducido por el usuario*/
        fscanf(stdin, "%s", buffer->nombre);
        buffer->id++;
        fprintf(stdout, "enviando la sennal!\n");
        fflush(stdout);
        /*envio la sennal a mi padre*/
        kill(getppid(), SIGUSR1);
        /*me desengancho de la memoria compartida*/
        shmdt((char*) buffer);
        exit(0);
    } else /*codigo del padre*/ {
        while (n_proc > 0)/*mientras quede algun proceso*/ {
            /*esperare a recibir alguna sennal (SIGUSR1)*/
            pause();
            /*imprimo el nombre y el id del cliente*/
            fprintf(stdout, "nombre: %s\t\tid: %d\n", buffer->nombre, buffer->id);
            fflush(stdout);
            if (wait(NULL) == -1)
                break;
            n_proc--;
        }
        /*me desengancho de la memoria compartida*/
        shmdt((char*) buffer);
        /*libero la memoria compartida*/
        shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
        exit(0);
    }
}
