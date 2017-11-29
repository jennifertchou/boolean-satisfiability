#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <stdbool.h>
#include "DPLL.h"
#include "formula.h"
#include "hset.h"

int main(int argc, char *argv[]) {

    int opt;
    bool verbose = false;
    char* formula_string = NULL;
    char* fileName = NULL;

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

    // Verify that the formula is in CNF.
    // TODO

    // Parse the formula and create the data structure.
    formula f;
    if (formula_string != NULL) {
        f = createFormula(formula_string);
    } else {
        f = createFormulaFromDIMACS(fileName);
    }
    if (verbose) {
        printf("Checking satisfiability of: ");
        printFormula(f);
    }

    bool satisfiable = DPLL(f);

    if (satisfiable) printf("SATISFIABLE\n");
    else printf("NOT SATISFIABLE\n");


    freeFormula(f);
    return 0;
}

bool DPLL(formula f) {
    printFormula(f);
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
            //printf("Found unit clause %c\n", lit->l);
            // Perform unit propgation: make this literal true 
            f = unit_propagate(f, lit->l, lit->intL, lit->negation);

            // Restart because we may have removed some clauses;
            return DPLL(f);
        }
        c = c->next;
    }
    
    // Pure literal elimination
    // If a literal occurs with only one polarity in the formula, it can
    // always be assigned in a way that makes all clauses containing them true,
    // so we can just erase that literal everywhere.
    // c = f->clauses;
    // bool foundPureLiteral = false;

    // while (c != NULL) {
    //     literal lit = c->literals;
    //     while (lit != NULL) {

    //         lit = lit->next;
    //     }
    //     c = c->next;
    // }

    // for every literal l that occurs pure in formula
    //     formula <-- pure_literal_assign(formula, l)


    // Just in case there are no more clauses or literals left.
    if (f->clauses == NULL || f->clauses->literals == NULL) {
        // The formula has no clauses, so it is satisfiable.
        return true;
    }

    // Splitting case, choose any literal and try making it true or false.
    literal lit = f->clauses->literals;
    return 
    DPLL(simplify(copyFormula(f), lit->l, lit->intL, 0)) 
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
    clause prevClause = NULL;
    clause c = f->clauses;
    while (c != NULL) {
        literal prevLit = NULL;
        literal lit = c->literals;
        bool removedClause = false;
        bool removedLiteral = false;
        while (lit != NULL) {
            // Check if this literal is the one we are setting to true.
            if (( (!f->isDIMACS && lit->l == l) || (f->isDIMACS && lit->intL == intL) )
                    && lit->negation == negation) {
                // Remove this entire clause, since the literal satisfies 
                // this clause.
                //printf("Removing the clause with %d, %c\n", negation, l);
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
