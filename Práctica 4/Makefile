CC = gcc -ansi -pedantic -D_SVID_SOURCE -D_XOPEN_SOURCE
CFLAGS = -Wall
OBJ= 
EXE = cadena_montaje ejercicio_final
LIBS = -lm -lrt
all : $(EXE)

.PHONY : clean
clean :
	rm -f ofiles/*.o core $(EXE)

$(EXE) : % : ofiles/%.o ofiles/semaforos.o
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ ofiles/$@.o ofiles/semaforos.o -pthread

ofiles/semaforos.o : src/semaforos.c src/semaforos.h src/types.h
	@echo "#---------------------------"
	@echo "# Generando semaforos.o"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $< -pthread
	mv *.o -f ofiles
ofiles/%.o : src/%.c src/semaforos.h  src/types.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $< -pthread
	mv *.o -f ofiles

folders :
	mkdir -v -p src
	mkdir -v -p ofiles
	mkdir -v -p doc
	mkdir -v -p txt

order :
	mv *.c src
	mv *.h src
	mv *.o ofiles
	mv *.txt txt

	

