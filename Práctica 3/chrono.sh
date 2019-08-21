#!/bin/bash

#argumentos de entrada:
#	1: numero de veces a ejecutar un comando
#	2: comando a ejecutar
#	3: fichero en el que guardar los datos
#Notese que el fichero en el que se guardan los datos primero es borrado por completo.
#Se ruega cuidado para evitar perder los datos de un archivo importante (por ejemplo, el código de algún .c)

i=0
#primero vamos a vaciar el fichero
echo "" > $3
#while( i < $1 ) // siendo $1 el primer argumento del script
#	ejecuto el comando pasado por $2 y guardo el tiempo en el fichero pasado por $3
#	i++
while [ $i -lt $1 ];do
	(time { $2 >/dev/null 2>&1; }) &>> $3 
	i=$(($i+1))
done
