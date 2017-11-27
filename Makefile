CC=gcc

CFLAGS=-g -Wall -std=c99 -O3

default: solver solver-test
	clear

solver: queues.c hset.c formula.c DPLL.c
	$(CC) $(CFLAGS) -o solver queues.c formula.c hset.c DPLL.c

solver-test: solver_test.c
	$(CC) $(CFLAGS) -o solver-test solver_test.c

clean: 
	$(RM) solver solver-test
