/**
 * @brief cabecera de la bilioteca de semaforos
 *
 * @file semaforos.h
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 15-04-2016
 */
#ifndef SEMAFOROS_H
#define SEMAFOROS_H
#include "types.h"
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
int Inicializar_Semaforo(int semid, unsigned short *array);

/***************************************************************
 *     Borrar_Semaforo.
 * @brief
 *     Borra un semaforo.
 * @param
 *     int semid: Identificador del semÃ¡foro.
 * @return
 *     int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Borrar_Semaforo(int semid);

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
int Crear_Semaforo(key_t key, int size, int *semid);

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
int Down_Semaforo(int id, int num_sem, int undo);

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
int DownMultiple_Semaforo(int id, int size, int undo, int *active);

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
int Up_Semaforo(int id, int num_sem, int undo);

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
int UpMultiple_Semaforo(int id, int size, int undo, int *active);
#endif
