#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <stdbool.h>
#include "DPLL.h"
#include "formula.h"

bool verbose = false;

int main(int argc, char *argv[]) {

    int opt;
    char* formula_string = NULL;
    char* fileName = NULL;

    // Parse command line flags.
    while ((opt = getopt(argc, argv, "hvi:f:")) != -1) {
        switch (opt){
            case 'h':
                printf("Usage: ./solver [-h] [-v] [-i x]/[-f p] -- program to determine whether "
                 "the given boolean formula is satisfiable or not.\n");
                printf("where:\n" 
                    "-h  show this help text\n"
                    "-v  verbose mode\n"
                    "-i  input formula such as '(a v ~b) ^ c ^ (~a v d)'\n"
                    "-f  path to file of test case in DIMACS cnf format\n");
                return 0;
                break;
            case 'v': 
                verbose = true;
                break;
            case 'i':
                formula_string = optarg;
                break;
            case 'f':
                fileName = optarg;
                break;
        }
    } 

    if (formula_string == NULL && fileName == NULL) {
        printf("Missing boolean formula input -- run ./solver -h for help\n");
        return 1;
    }

    if (formula_string != NULL && fileName != NULL) {
        printf("Found two inputs -- please use only one of -i or -f.\n");
        return 1;
    }

    // Parse the formula and create the data structure.
    formula f;
    if (formula_string != NULL) {
        f = createFormula(formula_string);
    } else {
        assert(fileName != NULL);
        f = createFormulaFromDIMACS(fileName);
    }

    if (f == NULL) {
        printf("Could not find file %s\n", fileName);
        return 1;
    }

    if (verbose) {
        printf("Checking satisfiability of: ");
        printFormula(f);
    }

    // Run the satisfiability algorithm.
    bool satisfiable;
    if (!f->isDIMACS) {
        satisfiable = DPLL(f);
    } else {
        char* assignment = malloc(sizeof(char) * (f->maxNumLiterals + 1));
        memset(assignment, 0, f->maxNumLiterals + 1);
        satisfiable = DPLL_DIMACS(f, assignment);
        if (satisfiable) {
            for (int i = 0; i < f->maxNumLiterals; i++) {
                if (assignment[i]) {
                    printf("%d ", i);
                }
            }
            printf("\n");
        }
        
        free(assignment);
    }

    if (satisfiable) printf("SATISFIABLE\n");
    else printf("NOT SATISFIABLE\n");

    freeFormula(f);
    return 0;
}

bool DPLL_DIMACS(formula f, char assignment[]) {
    if (verbose) printFormula(f);
    // If formula has no clauses, it is satisfiable.
    if (f->clauses == NULL) {
        //printf("No clauses\n");
        return true;
    }

    // If formula has an empty clause, it is not satisfiable.
    clause c = f->clauses;
    while (c != NULL) {
        if (c->literals == NULL) {
            //printf("Found empty clause\n");
            return false;
        }
        c = c->next;
    }

    // Unit propagation
    // If formula contains a clause with just one literal, make that literal
    // true.
    c = f->clauses;
    while (c != NULL) {
        // Check if this is a unit clause
        literal lit = c->literals;
        int var = lit->intL;
        if (lit != NULL && lit->next == NULL) {
            // Perform unit propgation: make this literal true
            f = unit_propagate(f, lit->l, lit->intL, lit->negation);

            // Restart because we may have removed some clauses
            if (DPLL_DIMACS(f, assignment)) {
                assignment[var] = 1;
                return true;
                
            } else {
                return false;
            }
            
        }
        c = c->next;
    }

    // Just in case there are no more clauses or literals left.
    if (f->clauses == NULL || f->clauses->literals == NULL) {
        // The formula has no clauses, so it is satisfiable.
        return true;
    }

    // Splitting case, choose any literal and try making it true or false.
    //printf("split case\n");
    literal lit = f->clauses->literals;
    int var = lit->intL;
    formula formula_copy = copyFormula(f);
    if (DPLL_DIMACS(simplify(formula_copy, lit->l, lit->intL, 0), assignment)) {
        assignment[var] = 1;
        freeFormula(formula_copy);
        return true;
    } else if (DPLL_DIMACS(simplify(f,lit->l, lit->intL, 1), assignment)) {
        assignment[var] = 1;
        freeFormula(formula_copy);
        return true;
    }
    freeFormula(formula_copy);
    return false;
}

bool DPLL(formula f) {
    if (verbose) printFormula(f);
    // If formula has no clauses, it is satisfiable.
    if (f->clauses == NULL) {
        //printf("No clauses\n");
        return true;
    }

    // If formula has an empty clause, it is not satisfiable.
    clause c = f->clauses;
    while (c != NULL) {
        if (c->literals == NULL) {
            //printf("Found empty clause\n");
            return false;
        }
        c = c->next;
    }

    // Unit propagation
    // If formula contains a clause with just one literal, make that literal
    // true.
    c = f->clauses;
    while (c != NULL) {
        // Check if this is a unit clause
        literal lit = c->literals;
        if (lit != NULL && lit->next == NULL) {
            // Perform unit propgation: make this literal true
            f = unit_propagate(f, lit->l, lit->intL, lit->negation);

            // Restart because we may have removed some clauses;
            return DPLL(f);
        }
        c = c->next;
    }

    // Just in case there are no more clauses or literals left.
    if (f->clauses == NULL || f->clauses->literals == NULL) {
        // The formula has no clauses, so it is satisfiable.
        return true;
    }

    // Splitting case, choose any literal and try making it true or false.
    //printf("split case\n");
    literal lit = f->clauses->literals;
    return DPLL(simplify(copyFormula(f), lit->l, lit->intL, 0))
            || DPLL(simplify(f,lit->l, lit->intL, 1));
}


// Unit propagation:
// If a clause is a unit clause (it contains only a single unassigned
// literal), this clause can only be satisfied by assigning the necessary 
// value to make this literal true. You can remove the negation of the literal
// from any clauses because it can't make its clause true. 
//
// If l is a unit clause, l should be made true.
// Remove clauses in the formula where l appears and remove ~l from
// clauses where it appears.
//
// If ~l is a unit clause, l should be made false.
// Remove clauses in the formula where ~l appears and remove l from
// clauses where it appears.
formula unit_propagate(formula f, char l, int intL, int negation) {
    //printf("in unit_propagate\n");
    // printFormula(f);
    clause prevClause = NULL;
    clause c = f->clauses;
    while (c != NULL) {
        literal prevLit = NULL;
        literal lit = c->literals;
        bool removedClause = false;
        while (lit != NULL) {
            bool removedLiteral = false;
            // Check if this literal is the one we are setting to true.
            if (( (!f->isDIMACS && lit->l == l) || (f->isDIMACS && lit->intL == intL) )
                    && lit->negation == negation) {
                // Remove this entire clause, since the literal satisfies 
                // this clause.

                //if (f->isDIMACS) printf("Removing the clause with %d, %d\n", negation, intL);
                //else printf("Removing the clause with %d, %c\n", negation, l);

                if (prevClause == NULL) {
                    f->clauses = c->next;
                    prevClause = NULL;
                } else {
                    prevClause->next = c->next;
                    // No change to prevClause.
                }
                removedClause = true;
                break;
            }
            // This literal is the negation of the desired literal.
            else if ((!f->isDIMACS && lit->l == l) || (f->isDIMACS && lit->intL == intL)) {
                // Remove the opposite of the literal from the clause because
                // it can't make this clause true.
                //printf("Removing the literal %d, %c\n", lit->negation, l);
                if (prevLit == NULL) {
                    c->literals = lit->next;
                    prevLit = NULL;
                } else {
                    prevLit->next = lit->next;
                }
                removedLiteral = true;
            }
            literal nextLit = lit->next;
            if (!removedLiteral) {
                prevLit = lit;
            } else {
                freeLiteral(lit);
            }
            lit = nextLit;
        }

        clause nextClause = c->next;
        if (!removedClause) {
            prevClause = c;
        } else {
            freeClause(c);
        }
        c = nextClause;
    }
    return f;
}

// Pure literal elimination:
// If a propositional variable occurs with only one polarity in 
// the formula, it is called pure. Pure literals can always be
// assigned in a way that makes all clauses containing them true.
// Thus, these clauses do not constrain the search anymore and can 
// be deleted. 
formula pure_literal_assign(formula f, char l) {
    return f;
}

formula simplify(formula f, char l, int intL, int negation) {
    // remove clauses in formula where l is positive
    // remove ~l from clauses where it appears
    // return new formula
    return unit_propagate(f, l, intL, negation);
}
