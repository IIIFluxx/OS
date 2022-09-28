# Makefile Variables
# AUTHOR: Bharath Sukesh - 19982634
# PURPOSE: Makefile to compile scheduler program

CC		 = gcc
EXEC	 = srtf
CFLAGS = -Wall -ansi -pedantic -pthread -g -std=c99
OBJ 	 = srtf.o
$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -g

srtf.o : srtf.c task.h
	$(CC) $(CFLAGS) -c srtf.c

clean:
	rm -f $(EXEC) $(OBJ)