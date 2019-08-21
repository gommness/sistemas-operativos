CC = gcc -ansi -pedantic
CFLAGS = -Wall
EXE = ejercicio4a ejercicio4b ejercicio5a ejercicio5b ejercicio6 ejercicio8a ejercicio8b ejercicio9 cosa
LIBS = -lm -lrt
all : $(EXE)

.PHONY : clean
clean :
	rm -f ofiles/*.o core $(EXE)

$(EXE) : % : ofiles/%.o
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -o $@ ofiles/$@.o

ofiles/%.o : src/%.c src/types.h
	@echo "#---------------------------"
	@echo "# Generando $@"
	@echo "# Depende de $^"
	@echo "# Ha cambiado $<"
	$(CC) $(CFLAGS) -c $<
	mv *.o -f ofiles

folders :
	mkdir -v -p src
	mkdir -v -p ofiles
	mkdir -v -p doc

order :
	mv *.c src
	mv *.h src
	mv *.o ofiles

	

