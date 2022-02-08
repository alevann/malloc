ifeq ($(OS),Windows_NT)
    CLEAN := del
else
    CLEAN := rm
endif


OUTNAME = malloctests

CC = gcc
CFLAGS = -g -Wall

clean:
	$(CLEAN) malloc.o
	$(CLEAN) $(OUTNAME).exe

malloc.o: malloc.c malloc.h
	$(CC) $(CFLAGS) -c malloc.c

malloctests: malloctests.c malloc.o malloc.h
	$(CC) $(CFLAGS) -o $(OUTNAME) malloctests.c malloc.o
