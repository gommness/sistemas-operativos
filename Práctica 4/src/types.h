/**
 * @brief Librerías
 *
 * Este modulo contiene las librerías para manejar procesos, hilos, señales, memoria compartida y semaforos.
 * @file includes.h
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#ifndef TYPES_H
#define TYPES_H

/*Estas macros evitan que nos den warnings al ejecutar el makefile
 hay que ponerlas antes de las librerías para que funcionen*/
#define _BSD_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <errno.h>
#include <fcntl.h>
#include "semaforos.h"
#define ERROR -1
#define SIZE 4096




#endif
