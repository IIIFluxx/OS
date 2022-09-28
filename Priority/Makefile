# Makefile Variables
# AUTHOR: Bharath Sukesh - 19982634
# PURPOSE: Makefile to compile scheduler program

CC		 = gcc
EXEC	 = pp
CFLAGS = -Wall -ansi -pedantic -pthread -g -std=c99
OBJ 	 = pp.o
$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -g

pp.o : pp.c task.h
	$(CC) $(CFLAGS) -c pp.c

clean:
	rm -f $(EXEC) $(OBJ)