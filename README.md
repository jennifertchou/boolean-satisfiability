# boolean-satisfiability
Implementation of a Boolean satisfiability problem (SAT) solver

Run `make` to create the executables `solver` and `run_solver_tests`.

Usage
------------
 `./solver [-h] [-v] [-i x]/[-f p]` -- program to determine whether the given boolean formula is satisfiable or not where:

-h  show help text

-v  verbose mode

-i  input CNF formula such as '(1 v -2) ^ 3 ^ (-1 v 4)', where the variables are increasing consecutive integers. Negative numbers denote the negations of the corresponding variables.

-f  path to file of test case in [DIMACS CNF format](http://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html)

Example Usage:
------------
`./solver -i "1 ^ (2 v -1) ^ (1 v 3 v 4)"`

`./solver -f sat_cases/uf50-0985.cnf`

Test Scripts:
------------
`./run_solver_tests` -- tests some satisfiable and not satisfiable formulas using `-i`

`./run_sat_cases` -- tests satisfiable formulas using `-f`

`./run_unsat_cases` -- tests unsatisfiable formulas using `-f`