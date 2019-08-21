/**
 * @brief implementacion de la biblioteca de semaforos
 *
 * @file ejercicio4.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */
#include "ejercicio4.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/***************************************************************
 *     Inicializar_Semaforo.
 * @brief
 *     Inicializa los semaforos indicados.
 * @param
 *     int semid: Identificador del semaforo.
 *     unsigned short *array: Valores iniciales.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ************************************************************/
int Inicializar_Semaforo(int semid, unsigned short *array) {
    union semun vals;
    vals.array = array;
    if (semctl(semid, 0, SETALL, vals) == -1)
        return errno;
    return 0;
}

/***************************************************************
 *     Borrar_Semaforo.
 * @brief
 *     Borra un semaforo.
 * @param
 *     int semid: Identificador del semÃ¡foro.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Borrar_Semaforo(int semid) {
    return semctl(semid, 0, IPC_RMID);
}

/***************************************************************
 *     Crear_Semaforo.
 * @brief
 *     Crea un semaforo con la clave y el tamaño
 *     especificado. Lo inicializa a 0.
 * @param
 *     key_t key: Clave precompartida del semaforo.
 *     int size: Tamaño del semaforo.
 * @return
 *     int *semid: Identificador del semaforo.
 *     int: ERROR en caso de error,
 *     0 si ha creado el semaforo,
 *     1 si ya estaba creado.
 **************************************************************/
int Crear_Semaforo(key_t key, int size, int *semid) {
    int output = 0;
    *semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if ((*semid == -1) && errno == EEXIST) {
        *semid = semget(key, size, SHM_R | SHM_W);
        output = 1;
    } else if (*semid == -1) {
        perror("semget");
        return ERROR;
    }
    return output;
}

/**************************************************************
 *     Down_Semaforo
 * @brief
 *     Baja el semaforo indicado
 * @param
 *     int semid: Identificador del semaforo.
 *     int num_sem: Semaforo dentro del array.
 *     int undo: Flag de modo persistente pese a finalización
 *     abrupta.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Down_Semaforo(int id, int num_sem, int undo) {
    struct sembuf sops;
    if(num_sem < 0){
        return ERROR;
    }
    sops.sem_num = (ushort) num_sem;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (undo == 1)
        sops.sem_flg = SEM_UNDO;
    return semop(id, &sops, 1);
}

/***************************************************************
 *     DownMultiple_Semaforo
 * @brief
 *     Baja todos los semaforos del array indicado
 *     por active.
 * @param
 *     int semid: Identificador del semaforo.
 *     int size: Numero de semaforos del array.
 *     int undo: Flag de modo persistente pese a finalización
 *     abrupta.
 *     int *active: Semaforos involucrados.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int DownMultiple_Semaforo(int id, int size, int undo, int *active) {
    int i; /*!< iterador*/
    int flag = 1; /*!< bandera que comprobara errores*/
    struct sembuf sops; /*!< estructura que almacenara las operaciones sobre los semaforos*/
    sops.sem_op = -1;
    sops.sem_flg = 0;
    if (undo == 1)
        sops.sem_flg = SEM_UNDO;
    for (i = 0; i < size; i++) {
        sops.sem_num = i;
        if (active[i] > 0) {
            flag = semop(id, &sops, active[i]);
            if (flag == -1)
                return -1;
        }
    }
    return flag;
}

/**************************************************************
 *     Up_Semaforo
 * @brief
 *     Sube el semaforo indicado
 * @param
 *     int semid: Identificador del semaforo.
 *     int num_sem: Semaforo dentro del array.
 *     int undo: Flag de modo persistente pese a finalizacion
 *     abupta.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Up_Semaforo(int id, int num_sem, int undo) {
    struct sembuf sops;
    if(num_sem < 0){
        return ERROR;
    }
    sops.sem_num = (ushort) num_sem;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (undo == 1)
        sops.sem_flg = SEM_UNDO;
    return semop(id, &sops, 1);
}

/***************************************************************
 *     UpMultiple_Semaforo
 * @brief
 *     Sube todos los semaforos del array indicado
 *     por active.
 * @param
 *     int semid: Identificador del semaforo.
 *     int size: Numero de semaforos del array.
 *     int undo: Flag de modo persistente pese a finalización
 * abrupta.
 *     int *active: Semaforos involucrados.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int UpMultiple_Semaforo(int id, int size, int undo, int *active) {
    int i;
    int flag = 1;
    struct sembuf sops;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    if (undo == 1)
        sops.sem_flg = SEM_UNDO;
    for (i = 0; i < size; i++) {
        sops.sem_num = i;
        if (active[i] > 0) {
            flag = semop(id, &sops, active[i]);
            if (flag == -1)
                return -1;
        }
    }
    return flag;
}

