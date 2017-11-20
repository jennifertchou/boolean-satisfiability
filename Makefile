CC=gcc

CFLAGS=-g -Wall -std=c99 -O3

default: solver

solver: queues.c hset.c util.c DPLL.c
	clear && $(CC) $(CFLAGS) -o solver queues.c util.c hset.c DPLL.c

clean: 
	$(RM) solver
