CC=gcc

CFLAGS=-g -Wall -std=c99 -O3

default: solver solver-test
	clear

solver: formula.c DPLL.c
	$(CC) $(CFLAGS) -o solver lib/queues.c formula.c DPLL.c

solver-test: solver_test.c
	$(CC) $(CFLAGS) -o run_solver_test solver_test.c

clean: 
	$(RM) solver run_solver_test
