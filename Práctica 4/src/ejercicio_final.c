/**
 * @brief simulación de la gestión de aulas durante un examen.
 *
 * @file ejercicio_final.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 21-04-2016
 */

#include "types.h"

#define SEMS 9 /*pensar cuantos semaforos necesito*/
#define NUM_AULAS 2
#define NUM_PROF 6 /* 2*(2 profesores + profesor examen) */
#define MAX_POS 10 /*tamano array de asientos maximo*/

typedef struct _Mensaje {
    long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
    int aula; /*0 si es del aula 0, 1 si es del aula 1*/
    int pid; /*pid del alumno*/
} mensaje;

struct info {
    int pids[6]; /*aqui guardo los pids de los profesores*/
    int successes; /*numero de alumnos que han hecho el examen*/
    int posicion1[MAX_POS]; /*array de asientos de aula 0*/
    int posicion2[MAX_POS]; /*array de asientos aula 1*/
    int contador[2]; /*contador de alumnos en aula 0 o 1*/
};
int CHRONO = 0; /*esta variable se utilizara para coontabilizar el numero de veces que el padre ha enviado la señal de los 30 segundos*/
int examFlag = 1; /*flag que indicara a los profesores recogedores si el examen continua o no*/
int id_zone; /*id de la zona de memoria compartida*/
int semid; /*id de los semaforos*/
int msqid; /*id de la cola de mensajes*/
int file; /*fichero donde se imprimen los procesos*/

/**
 * @brief imprime un array de int de tamano N
 * @param arr el array 
 * @param N el tamano del array
 */
void print_T(int* arr, int N) {
    int i;
    printf("[ ");
    for (i = 0; i < N; i++) {
        printf("%d ", arr[i]);
    }
    printf("]");
}
/**
 * @brief mata los procesos profesores menos a mi mismo 
 * @param semid el identificador de los semaforos
 * @param buffer la memoria compartida 
 * @param me la i asociada al profesor
 */
void killAll(int semid, struct info* buffer, int me){
    int i;
    Down_Semaforo(semid,8,1);
    for(i=0;i<6;i++){
        if(i != me){
        kill(buffer->pids[i],SIGINT);
        }
    }
    Up_Semaforo(semid,8,1);
}

/**
 * @brief manejador de la señal SIGUSR1 que imprime un mensaje cuando se captura dicha señal
 * @param sig la señal a manejar
 */
void manejador_SIGUSR1(int sig) {
    printf("sennal SIGUSR1 enviada\n");
    fflush(stdout);
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }
}

/**
 * @brief manejador de la señal SIGUSR2 que sirve para notificar a los alumnos de que el examen ha finalizado
 * @param sig la señal a manejar
 */
void manejador_SIGUSR2(int sig) {
    examFlag = 0;
    if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
        puts("Error en la captura");
        exit(1);
    }
}

/**
 * @brief manejador de la señal SIGINT que imprime un mensaje cuando se captura dicha señal y libera todos los recursos
 * @param sig la señal a manejar
 */
void manejador_SIGINT(int sig) {
    while (wait(NULL) != -1); /*si el proceso tiene hijos, les espera*/
    close(file);
    Borrar_Semaforo(semid); /*elimina los semaforos*/
    msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
    shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL); /*elimina memoria compartida*/
    exit(EXIT_FAILURE); /*finalizacion del proceso*/
}

/**
 * @brief manejador de la señal SIGALRM que se dedica a avisar de que el examen se acaba
 * @param sig la señal a manejar
 */
void manejador_SIGALRM_FIN(int sig) {
    if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
        puts("Error en la captura");
        exit(1);
    }

    kill((-1) * getpid(), SIGUSR2);

}

/**
 * @brief manejador de la señal SIGALRM que se dedica a imprimir cada 30s los procesos hijos vivos
 * @param sig la señal a manejar
 */
void manejador_SIGALRM_gestor(int sig) {
    char command[80];
    CHRONO++;

    sprintf(command,"ps -ef | grep %d | grep -v grep",(int)getpid());
    system(command);
    /*Cuando queden 30s de examen, arma un manejador diferente para acabar el examen*/
    if (CHRONO == 9) {
        if (signal(SIGALRM, manejador_SIGALRM_FIN) == SIG_ERR) {
            puts("Error en la captura");
            exit(1);
        }
    }

    if (alarm(30)) {
        fprintf(stderr, "Existe una alarma previa establecida\n");
    }
}

/**
 * @brief manejador de la señal SIGALRM que se dedica a avisar de que el alumno ha terminado el examen
 * @param sig la señal a manejar
 */
void manejador_SIGALRM_EXAM(int sig) {
    printf("TERMINO\n");
}

int main(int argc, char** argv) {
    int emptyFlag = 0; /*flag que indicara si las mesas de las aulas estan vacias*/
    int tam_aula[2]; /*tamano de cada aula*/
    double tam_ocupado[2]; /*tamano de cada aula que se puede ocupar*/
    int num_alumnos; /*numero de alumnos totales*/
    int i, j,flag;
    int pid; /*pid del proceso*/
    int asig_aula; /*indica el aula al que pertenece un alumno 0, si aula 0 o 1 si aula 1*/
    int key; /*clave para crear semaforos, memoria compartida y colas de mensajes*/
    unsigned short sem[SEMS]; /*!< el array de valores iniciales que utilizare para inicializar los semaforos*/
    struct info* buffer; /*!< estructura con los campos deseados para la memoria compartida*/
    mensaje msg;
    /*asigno el manejador para la sennal SIGINT*/

    if (signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        puts("Error en la captura");
        exit(1);
    }

    /*asigno el manejador para la sennal SIGUSR1*/
    if (signal(SIGUSR1, manejador_SIGUSR1) == SIG_ERR) {
        puts("Error en la captura2");
        exit(1);
    }
    printf("Introduzca numero de asientos en el primer aula:\n");
    scanf("%d", &tam_aula[0]);
    printf("Introduzca numero de asientos en el segundo aula:\n");
    scanf("%d", &tam_aula[1]);
    printf("Introduzca numero de alumnos:\n");
    scanf("%d", &num_alumnos);
    /*Aqui comprobamos si los valores de simulacion pasados son los optimos para el problema*/
    if (tam_aula[0] < tam_aula[1]) {
        if (!(num_alumnos > tam_aula[1] && num_alumnos < (tam_aula[0] + tam_aula[1]))) {
            perror("Valores de simulacion no efectivos");
            return ERROR;
        }

    } else {
        if (!(num_alumnos > tam_aula[0] && num_alumnos < (tam_aula[0] + tam_aula[1]))) {
            perror("Valores de simulacion no efectivos");
            return ERROR;
        }
    }
    /*Calculamos los tamanos que se pueden ocupar en cada aula*/
    tam_ocupado[0] = 0.85 * tam_aula[0];
    tam_ocupado[1] = 0.85 * tam_aula[1];

    key = ftok("ejercicio_final", 2700);
    if (key == -1) {
        fprintf(stderr, "error con la clave");
        exit(-1);
    }

    /*obtengo el id de la zona de memoria que sera compartida*/
    id_zone = shmget(key, sizeof (struct info)*1, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone == -1) {
        id_zone = shmget(key, sizeof (struct info)*1, SHM_R | SHM_W);
        if (id_zone == -1) {
            fprintf(stderr, "error al obtener memoria compartida");
            exit(-1);
        }
    }
    /*Creo la cola de mensajes*/
    msqid = msgget(key, 0600 | IPC_CREAT);
    if (msqid == -1) {
        perror("Error al obtener identificador para cola mensajes");
        return (0);
    }
    /*Creo los semaforos*/
    if (Crear_Semaforo(key, SEMS, &semid) == -1) {
        fprintf(stdout, "error al crear semaforos\n");
        exit(EXIT_FAILURE);
    }
    sem[0] = 1; /*Este es el semaforo de acceso al aula 0*/
    sem[1] = 1; /*Este es el semaforo de acceso al aula 1*/
    sem[2] = 1; /*Este es el semaforo de zona de asignacion de aula*/
    sem[3] = 1; /*Este es el semaforo de posiciones del aula 0*/
    sem[4] = 1; /*Este es el semaforo de posiciones del aula 1*/
    sem[5] = 1; /*Este es el semaforo para los msgreceive de profesores del aula 0*/
    sem[6] = 1; /*Este es el semaforo para los msgreceive de profesores del aula 1*/
    sem[7] = 1; /*Este semaforo protege la variable compartida successes*/
    sem[8] = 1; /*Este es el semaforo de pids de la memoria compartida*/
    /*inicializo los semaforos con sus valores*/
    if (Inicializar_Semaforo(semid, sem) != 0) {
        fprintf(stderr, "error al inicializar los semaforos");
        printf("%d", errno);
        kill(getpid(), SIGINT);
    }
    /*CODIGO COMUN PARA ENGANCHAR LA MEMORIA COMPARTIDA*/
    buffer = shmat(id_zone, (struct info *) 0, 0);
    if (buffer == NULL) {
        fprintf(stderr, "error al enganchar la memoria compartida");
        while (wait(NULL) != -1); /*si el padre tiene este error, espera a todos sus hijos*/

        shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
        exit(-1);
    }
    /*inicializo el buffer compartido*/
    buffer->contador[0] = 0;
    buffer->contador[1] = 0;
    for (j = 0; j < MAX_POS; j++) {
        buffer->posicion1[j] = -1;
    }
    for (j = 0; j < MAX_POS; j++) {
        buffer->posicion2[j] = -1;
    }

    for (i = 0; i < (num_alumnos + NUM_PROF); i++) {
        if ((pid = fork()) == -1) {
            perror("Error en el fork\n");
            return ERROR;
        } else if (pid == 0) {
            /*Profesores*/
            if (i < 6) {
                break;
                /*alumnos*/
            } else {
                srand(getpid());
                asig_aula = rand() % 2;
                sleep(rand() % 5);
                break;
            }
        }
    }

    /*Codigo de los hijos (alumnos y profesores)*/
    if (pid == 0) {
        /*codigo de los alumnos (hijos)*/
        if (i > 5) {
            if (signal(SIGALRM, manejador_SIGALRM_EXAM) == SIG_ERR) {
                puts("Error en la captura2");
                exit(1);
            }

            /*Si no ha entrado en el aula y le mandan esa senal (ha acabado el tiempo de examen), el proceso acaba sin hacer el examen*/
            if (signal(SIGUSR2, manejador_SIGINT) == SIG_ERR) {
                puts("Error en la captura2");
                exit(1);
            }
            /*Espera a que le asignen un aula*/
            fflush(NULL);
            Down_Semaforo(semid, 2, 1);

            /*Se queda esperando mientras las aulas esten a mas del 85% de ocupacion*/
            flag = 0;
            while (buffer->contador[asig_aula] >= tam_ocupado[asig_aula]) {
                asig_aula = (asig_aula + 1) % 2;
                if (flag == 0) {
                    printf("ALUMNO %d INTENTA OTRO AULA\n", getpid());
                    flag = 1;
                }
            }
            /*Entrar en aula*/

            Down_Semaforo(semid, asig_aula, 1);
            printf("ALUMNO %d ENTRA EN AULA %d\n", getpid(), asig_aula);
            buffer->contador[asig_aula]++;
            Up_Semaforo(semid, asig_aula, 1);

            Up_Semaforo(semid, 2, 1);
            /*Avisa a padre de que espera un profesor para que lo siente*/
            msg.id = 1;
            msg.aula = asig_aula;
            msg.pid = getpid();
            printf("ALUMNO %d ESPERO ASIENTO\n", getpid());
            fflush(stdout);
            if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
                printf("ERROR al enviar\n");
            }
            pause(); /*me han sentado*/
            /*sobreescribo el manejador por si se acaba el tiempo del examen antes de que yo termine*/
            if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
                puts("Error en la captura2");
                exit(1);
            }
            /*Hace el examen un tiempo aleatorio*/
            printf("ALUMNO %d HAGO EL EXAMEN\n", getpid());
            sleep(rand() % 5); /*termino el examen*/

            printf("ALUMNO %d TERMINA EXAMEN\n", getpid());
            /*una vez acaba manda un mensaje de tipo 2 para avisar al profesor
             del examen para que le recoja el examen*/
            /*mensaje para salir del examen*/
            if (asig_aula == 0) {
                msg.id = 5;
            } else if (asig_aula == 1) {
                msg.id = 6;
            }
            msg.pid = getpid();

            printf("ALUMNO %d ESPERA PERMISO SALIR\n", getpid());
            if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
                printf("ERROR al enviar\n");
            }
            pause();
            printf("ALUMNO %d SALE\n", getpid());
            Down_Semaforo(semid, asig_aula, 1);
            buffer->contador[asig_aula]--;
            Up_Semaforo(semid, asig_aula, 1);

            /*me desengancho de la memoria compartida*/
            shmdt((char*) buffer);



        }/*Codigo de los profesores*/
        else {

            Down_Semaforo(semid,8,1);
            buffer->pids[i] = getpid();
            Up_Semaforo(semid,8,1);
            while (1) {

                if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
                    puts("Error en la captura");
                    exit(1);
                }

                /*profesor examen aula0*/
                if (i == 0) {
                    if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 5, 0) == -1) {
                        printf("ERROR al recibir examen 0\n");
                    }
                    if (msg.pid != -1) {
                        Down_Semaforo(semid, 3, 1);

                        printf("\nPROFESOR %d BUSCA EXAMEN %d AULA = %d\n\n", getpid(), msg.pid, msg.aula);
                        printf("Tabla 0: ");
                        print_T(buffer->posicion1, MAX_POS);
                        printf("\n");
                        /*busca el asiento del que ha acabado el examen*/
                        for (j = 0; buffer->posicion1[j] != msg.pid && j < MAX_POS; j++);
                        printf("PROFESOR %d OBTIENE EXAMEN %d\n", getpid(), msg.pid);
                        /*Deja dicha posicion libre*/
                        buffer->posicion1[j] = -1;
                        printf("Tabla 0: ");
                        print_T(buffer->posicion1, MAX_POS);
                        printf("\n");
                        /*Avisa al alumno de que puede irse*/
                        kill(msg.pid, SIGUSR1);
            			msg.pid = -1;
                        Up_Semaforo(semid, 3, 1);
                    }
                    Down_Semaforo(semid, 7, 1);
                    buffer->successes++;
                    Up_Semaforo(semid, 7, 1);

                    Down_Semaforo(semid, 4, 1);
                    Down_Semaforo(semid, 3, 1);
                    Down_Semaforo(semid, 7, 1);
                    if (examFlag == 0) {
                        emptyFlag = 1;
                        for (j = 0; j < MAX_POS; j++) {
                            if (buffer->posicion1[j] != -1) {
                                emptyFlag = 0;
                                break;
                            }
                        }
                    }
                    if (emptyFlag == 1 || buffer->successes >= num_alumnos) {
                        killAll(semid,buffer,i);
                        Up_Semaforo(semid, 7, 1);
                        Up_Semaforo(semid, 3, 1);
                        Up_Semaforo(semid, 4, 1);
                        exit(0);
                    }
                    Up_Semaforo(semid, 7, 1);
                    Up_Semaforo(semid, 3, 1);
                    Up_Semaforo(semid, 4, 1);

                }/*profesor examen aula 1*/
                else if (i == 1) {
                    if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 6, 0) == -1) {
                        printf("ERROR al recibir examen 1\n");
                    }
                    if (msg.pid != -1) {
                        Down_Semaforo(semid, 4, 1);

                        printf("\nPROFESOR %d BUSCA EXAMEN %d AULA = %d\n\n", getpid(), msg.pid, msg.aula);
                        printf("Tabla 1: ");
                        print_T(buffer->posicion2, MAX_POS);
                        printf("\n");
                        for (j = 0; buffer->posicion2[j] != msg.pid && j < MAX_POS; j++);
                        printf("PROFESOR %d OBTIENE EXAMEN %d\n", getpid(), msg.pid);
                        buffer->posicion2[j] = -1;
                        Down_Semaforo(semid, 7, 1);
                        buffer->successes++;
                        Up_Semaforo(semid, 7, 1);

                        printf("Tabla 1: ");
                        print_T(buffer->posicion2, MAX_POS);
                        printf("\n");
                        kill(msg.pid, SIGUSR1);
            			msg.pid = -1;
                        Up_Semaforo(semid, 4, 1);
                    }
                    Down_Semaforo(semid, 4, 1);
                    Down_Semaforo(semid, 3, 1);
                    Down_Semaforo(semid, 7, 1);
                    if (examFlag == 0) {
                        emptyFlag = 1;
                        for (j = 0; j < MAX_POS; j++) {
                            if (buffer->posicion2[j] != -1) {
                                emptyFlag = 0;
                                break;
                            }
                        }
                    }
                    if (emptyFlag == 1 || buffer->successes >= num_alumnos) {
                        killAll(semid,buffer,i);
                        Up_Semaforo(semid, 7, 1);
                        Up_Semaforo(semid, 3, 1);
                        Up_Semaforo(semid, 4, 1);
                        exit(0);
                    }
                    Up_Semaforo(semid, 7, 1);
                    Up_Semaforo(semid, 3, 1);
                    Up_Semaforo(semid, 4, 1);

                }/*profesores aula 0*/
                else if (i == 2 || i == 3) {

                    if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
                        puts("Error en la captura");
                        exit(1);
                    }

                    Down_Semaforo(semid, 5, 1);
                    if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 3, 0) == -1) {
                        printf("ERROR al recibir mensaje de asiento 0\n");
                    }
                    Up_Semaforo(semid, 5, 1);

                    Down_Semaforo(semid, 3, 1);
                    for (j = 0; buffer->posicion1[j] != -1 && j < MAX_POS; j++);
                    buffer->posicion1[j] = msg.pid;
                    Up_Semaforo(semid, 3, 1);
                    printf("ACOMODADOR %d METE A %d\n", getpid(), msg.pid);
                    printf("Tabla 0: ");
                    print_T(buffer->posicion1, MAX_POS);
                    printf("\n");
                    kill(msg.pid, SIGUSR1);

                }/*profesores aula 1*/
                else if (i == 4 || i == 5) {

                    if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
                        puts("Error en la captura");
                        exit(1);
                    }

                    Down_Semaforo(semid, 6, 1);
                    if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 4, 0) == -1) {
                        printf("ERROR al recibir mensaje de asiento 1\n");
                    }
                    Up_Semaforo(semid, 6, 1);

                    Down_Semaforo(semid, 4, 1);
                    for (j = 0; buffer->posicion2[j] != -1 && j < MAX_POS; j++);
                    buffer->posicion2[j] = msg.pid;
                    printf("ACOMODADOR %d METE A %d\n", getpid(), msg.pid);
                    printf("Tabla 1: ");
                    print_T(buffer->posicion2, MAX_POS);
                    printf("\n");
                    Up_Semaforo(semid, 4, 1);
                    kill(msg.pid, SIGUSR1);

                }
            }
            shmdt((char*) buffer);
        }
    } else { /*codigo del padre, el gestor*/
        file = open("SIGHUP_PPID_lista_proc.txt", O_CREAT | O_RDWR | O_TRUNC);
        if(file == -1){
            printf("\n\nerror al abrir el fichero\npor favor, borre el archivo SIGHUP_PPID_lista_proc.txt si existe en su sistema\n\n\n");
        }
        dup2(file,1);
        if (signal(SIGALRM, manejador_SIGALRM_gestor) == SIG_ERR) {
            puts("Error en la captura");
            exit(EXIT_FAILURE);
        }
        if (alarm(30)) {
            fprintf(stderr, "Existe una alarma previa establecida\n");
        }

        if (signal(SIGUSR2, manejador_SIGUSR2) == SIG_ERR) {
            puts("Error en la captura");
            exit(1);
        }

        while (1) {
            /*Aqui el gestor se encarga principalmente de comunicar al alumno que quiere sentarse al profesor de su aula correspondiente para que lo siente*/
            if (msgrcv(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), 1, 0) == -1) {
                kill(-1*getpid(),SIGINT);
            }
            if (msg.aula == 0) {
                msg.id = 3; /*Mensaje a profesor aula 0*/
            } else {
                msg.id = 4; /*mensaje a profesor aula 1*/
            }
            if (msgsnd(msqid, (struct msgbuf *) &msg, sizeof (mensaje) - sizeof (long), IPC_NOWAIT) == -1) {
                /*printf("ERROR al enviar\n");*/
            }
            Down_Semaforo(semid, 7, 1);
            if (buffer->successes >= num_alumnos)
                kill((-1) * getpid(), SIGUSR2);
            Up_Semaforo(semid, 7, 1);
        }
        while (wait(NULL) != -1);
        Borrar_Semaforo(semid); /*libero los semaforos*/
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        /*libero la memoria compartida*/
        shmctl(id_zone, IPC_RMID, (struct shmid_ds *) NULL);
    }

    close(file);
    return (EXIT_SUCCESS);
}
