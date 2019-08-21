/**
 * @brief Librerías de procesos.
 *
 * Este modulo contiene las librerías para manejar procesos, hilos y señales.
 * @file types.h
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#ifndef TYPES_H
#define	TYPES_H
/*Estas macros evitan que nos den warnings al ejecutar el makefile
 hay que ponerlas antes de las librerías para que funcionen*/
#define _BSD_SOURCE
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#endif	/* TYPES_H */

