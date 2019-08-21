/**
 * @brief Ejercicio de comunicacion de procesos mediante pipes
 * @file ejercicio9.c
 * @author Carlos Li Hu             &lt;carlos.li@estudiante.uam.es&gt;
 * @author Javier Gómez Martínez    &lt;javier.gomezmartinez@estudiante.uam.es&gt;
 * @version 1.0
 * @date 19-02-2016
 */

#include "types.h"
#define READ 0
#define WRITE 1

/*
 * 
 */
int main() {
    /*tuberias cuyo flujo de datos sera desde padres a hijos*/
    int p_padre_hijo[2], fd_padre_hijo[2][2], p_hijo_nieto[2], fd_hijo_nieto[2][2];
    /*tuberias cuyo flujo de datos sera desde hijos a padres*/
    int p_hijo_padre[2], fd_hijo_padre[2][2], p_nieto_hijo[2], fd_nieto_hijo[2][2];
    /*enteros necesarios:
        pid: tendra los valores devueltos por el fork()
        ppid: tendra el valor del padre de todos los procesos
        nbytes: recibira el valor devuelto por read()
        marca_hijo: almacenara un valor que le indicara al hijo si es el primero (0) o el segundo (1)
        marca_nieto: almacenara un valor que le indicara al nieto si es el primero (0) o el segundo (1)
     
     notese que para los nietos, solo interesa saber si son hijo 0 ó hijo 1 de su proceso padre. No es necesario
     distinguirse de sus procesos "primos" (hijos del hermano de su padre)
     */
    int pid, ppid, nbytes, marca_hijo, marca_nieto;
    /*la variable donde se van a leer los datos pasados por las tuberias*/
    char readBuffer[160];
    /*la variable donde se van a escribir los datos para posteriormente pasarlos*/
    char * string;
    

    ppid = getpid();/*almaceno el PID del padre para más tarde diferenciar 
                     a los hijos de los nietos*/
    pid = 0;
    p_padre_hijo[0] = pipe(fd_padre_hijo[0]);
    p_hijo_padre[0] = pipe(fd_hijo_padre[0]);
    marca_hijo = 0;
    if(p_padre_hijo[0] == -1 || p_hijo_padre[0] == -1){
        fprintf(stderr,"error al abrir una tuberia\n");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid == -1){
        fprintf(stderr,"error al crear un proceso hijo\n");
        exit(EXIT_FAILURE);
    } else if(pid!=0){
        close(fd_hijo_padre[0][WRITE]);/*cierro el escritor hijo-padre padre a hijo*/
        close(fd_padre_hijo[0][READ]);/*cierro el lector padre-hijo desde padre a hijo 0*/
        p_hijo_padre[1] = pipe(fd_hijo_padre[1]);
        p_padre_hijo[1] = pipe(fd_padre_hijo[1]);
        if(p_hijo_padre[1] == -1 || p_padre_hijo[1] == -1){
            fprintf(stderr,"error al abrir una tuberia\n");
            exit(EXIT_FAILURE);
        }
        marca_hijo = 1;
        pid = fork();
        if(pid == -1){
            fprintf(stderr,"error al crear un proceso hijo\n");
            exit(EXIT_FAILURE);
        } else if(pid != 0){
            close(fd_hijo_padre[1][WRITE]);/*cierro el escritor hijo-padre desde padre a hijo 1*/
            close(fd_padre_hijo[1][READ]);/*cierro el lector padre-hijo desde padre a hijo 1*/
        } else {
            close(fd_hijo_padre[1][READ]);/*cierro el lector hijo-padre desde hijo 1 a padre*/
            close(fd_padre_hijo[1][WRITE]);/*cierro el escritor padre-hijo desde hijo 1 a padre*/
        }
    } else {
        close(fd_hijo_padre[0][READ]);/*cierro el lector hijo-padre desde hijo 0 a padre*/
        close(fd_padre_hijo[0][WRITE]);/*cierro el escritor padre-hijo desde hijo 0 a padre*/
    }
    
    if(pid == 0){
        p_nieto_hijo[0] = pipe(fd_nieto_hijo[0]);
        p_hijo_nieto[0] = pipe(fd_hijo_nieto[0]);
        if(p_nieto_hijo[0] == -1 || p_hijo_nieto[0] == -1){
            fprintf(stderr,"error al abrir una tuberia\n");
            exit(EXIT_FAILURE);
        }
        marca_nieto = 0;
        pid = fork();
        if(pid == -1){
            fprintf(stderr,"error al crear un proceso hijo\n");
            exit(EXIT_FAILURE);
        } else if(pid != 0){
            close(fd_nieto_hijo[0][WRITE]);/*cierro el escritor nieto-hijo desde hijo a nieto 0*/
            close(fd_hijo_nieto[0][READ]);/*cierro el lector hijo-nieto desde hijo nieto 0*/
            p_nieto_hijo[1] = pipe(fd_nieto_hijo[1]);
            p_hijo_nieto[1] = pipe(fd_hijo_nieto[1]);
            if(p_nieto_hijo[1] == -1 || p_hijo_nieto[1] == -1){
                fprintf(stderr,"error al abrir una tuberia\n");
                exit(EXIT_FAILURE);
            }
            marca_nieto = 1;
            pid = fork();
            if(pid != 0){
                close(fd_nieto_hijo[1][WRITE]);/*cierro el escritor nieto-hijo desde hijo a nieto 1*/
                close(fd_hijo_nieto[1][READ]);/*cierro el lector hijo-nieto desde hijo a nieto 1*/
            } else {
                close(fd_nieto_hijo[1][READ]);/*cierro el lector nieto-hijo desde nieto 1 a hijo*/
                close(fd_hijo_nieto[1][WRITE]);/*cierro el escritor hijo-nieto desde nieto 1 a hijo*/
            }
        } else {
            close(fd_nieto_hijo[0][READ]);/*cierro el lector nieto-hijo desde nieto 0 a hijo*/
            close(fd_hijo_nieto[0][WRITE]);/*cierro el escritor hijo-nieto desde nieto 0 a hijo*/
        }
    }
    /*hasta aquí entonces tendríamos 1 proceso padre, con 2 procesos hijos y
     cada uno de ellos con otros 2 procesos hijos. En otras palabras:
     1 padre, 2 hijos y 4 nietos
     Tambien tendremos 2 tuberias (para cada padre) que conectan cada padre con sus hijos listas para que
     cada padre le escriba algo a sus hijos
     y otras 2 tuberias (para cada padre) que conectan cada hijo con su padre para que cada hijo le escriba a su padre*/
    
    string = (char*)malloc(sizeof(char)*160);/*esta reserva de memoria esta colocada en codigo comun
                                             para todos los procesos a propósito, pues todos van a hacer uso de ella*/
    if(string == NULL){
        fprintf(stderr,"error al reservar memoria\n");
        exit(EXIT_FAILURE);
    }
    
    /*AQUI ENTRAMOS EN EL CODIGO DEL PADRE DE TODOS*/
    if(ppid == getpid()){
        sprintf(string,"Datos enviados a traves de la tuberia\n");
        write(fd_padre_hijo[0][WRITE], string, strlen(string));
        write(fd_padre_hijo[1][WRITE], string, strlen(string));
        close(fd_padre_hijo[0][WRITE]);
        close(fd_padre_hijo[1][WRITE]);
        /*envio a los hijos el string que quería y despues cierro las tuberias-escritura del proceso padre*/
    } else if(ppid == getppid()){/*AQUI EN EL CODIGO DE LOS HIJOS*/
        nbytes = read(fd_padre_hijo[marca_hijo][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_padre_hijo[marca_hijo][READ]);
        printf("%s",readBuffer);
        /*leo los datos que me pasa mi padre y cierro la tuberia por donde los he leido*/
        sprintf(string,"Datos enviados a traves de la tuberia por el proceso PID=%d\n",getpid());
        write(fd_hijo_nieto[0][WRITE], string, strlen(string));
        write(fd_hijo_nieto[1][WRITE], string, strlen(string));
        /*a traves de mis tuberias de escritura, le envio los datos a los nietos y luego las cierro*/
    } else {/*AQUI EN EL CODIGO DE LOS NIETOS*/
        nbytes = read(fd_hijo_nieto[marca_nieto][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_hijo_nieto[marca_nieto][READ]);
        printf("%s",readBuffer);
        /*leo los datos que me pasa mi padre (soy nieto) y cierro la tuberia por donde los he leido*/
        sprintf(string,"Datos devueltos a traves de la tuberia por el proceso PID=%d\n",getpid());
        write(fd_nieto_hijo[marca_nieto][WRITE], string, strlen(string));
        close(fd_nieto_hijo[marca_nieto][WRITE]);
        /*devuelvo datos a mi padre y cierro la tuberia por donde los devuelvo*/
    }
    /*DE VUELTA AL CODIGO DE LOS HIJOS*/
    if(ppid == getppid()){
        while(wait(NULL) != -1);
        nbytes = read(fd_nieto_hijo[0][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_nieto_hijo[0][READ]);
        /*recibo datos de uno de mis hijos y cierro la tuberia*/
        write(fd_hijo_padre[marca_hijo][WRITE], readBuffer, strlen(readBuffer));
        nbytes = read(fd_nieto_hijo[1][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_nieto_hijo[1][READ]);
        /*recibo datos del otro de mis hijos y cierro tambien la tuberia*/
        write(fd_hijo_padre[marca_hijo][WRITE], readBuffer, strlen(readBuffer));
        close(fd_hijo_padre[marca_hijo][WRITE]);
        /*devuelvo datos a mi padre y cierro la tuberia por donde enviare esos datos*/
    } else if(ppid == getpid()){/*DE VUELTA AL CODIGO DEL PADRE*/
        while(wait(NULL) != -1);
        nbytes = read(fd_hijo_padre[0][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_hijo_padre[0][READ]);
        printf("%s",readBuffer);
        /*leo los datos que me pasa uno de mis hijos y cierro esa tuberia*/
        nbytes = read(fd_hijo_padre[1][READ], readBuffer, sizeof(readBuffer));
        if(nbytes == -1){
            fprintf(stderr,"error al leer de la tuberia\n");
            exit(EXIT_FAILURE);
        }
        close(fd_hijo_padre[1][READ]);
        printf("%s",readBuffer);
        /*leo los datos que me pasa mi otro hijo y cierro la tuberia*/
    }
    
    free(string);
    
    exit(EXIT_SUCCESS);
}

