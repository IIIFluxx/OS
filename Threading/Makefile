# Makefile Variables
# AUTHOR: Bharath Sukesh - 19982634
# PURPOSE: Makefile to compile threading program
CC		 = gcc
EXEC	 = schedsim
CFLAGS = -Wall -ansi -pedantic -pthread -g -std=c99
OBJ 	 = schedsim.o
$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -g -pthread

schedsim.o : schedsim.c task.h
	$(CC) $(CFLAGS) -c schedsim.c

clean:
	rm -f $(EXEC) $(OBJ)