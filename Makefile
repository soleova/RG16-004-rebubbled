PROGRAM = main
CC      = gcc
CFLAGS  = -g -Wall 
LDLIBS  = -lglut -lGLU -lGL -lm

$(PROGRAM): main.o
	$(CC) -o $(PROGRAM) main.o $(LDLIBS)

.PHONY: clean dist

clean:
	-rm -rf *.o 
	-rm -rf *~
	-rm -rf *.c~
