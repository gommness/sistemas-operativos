CC = gcc -ansi -pedantic -D_SVID_SOURCE -D_XOPEN_SOURCE
CFLAGS = -Wall
EXE = ejercicio2 ejercicio5 ejercicio6 ejercicio7
OBJ = 
LIBS = -lm -lrt
all : $(EXE)

.PHONY : clean
clean :
	rm -f ofiles/*.o core $(EXE)

$(EXE) : % : ofiles/%.o ofiles/ejercicio4.o
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ ofiles/$@.o ofiles/ejercicio4.o -pthread

ofiles/ejercicio4.o : src/ejercicio4.c src/ejercicio4.h src/includes.h
	@echo "#---------------------------"
	@echo "# Generando ejercicio4.o"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $< -pthread
	mv *.o -f ofiles
ofiles/%.o : src/%.c src/ejercicio4.h  src/includes.h
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

order :
	mv *.c src
	mv *.h src
	mv *.o ofiles

	

