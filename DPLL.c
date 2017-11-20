#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "DPLL.h"
#include "util.h"
#include "hset.h"

int main(int argc, char *argv[]) {
    // Check if arguments are valid.
    if (argc <= 1) {
        printf("Please enter the boolean formula in CNF as the argument, "
            "like: ./solver '(a v ~b) ^ c ^ (~a v d)'\n");
        return 1;
    }
    if (argc >= 3) {
        printf("Too many arguments entered\n"
            "Please enter only the boolean formula in CNF as the argument, "
            "like: ./solver '(a v ~b) ^ c ^ (~a v d)'\n");
        return 1;
    }

    char* formula_string = argv[1];
    printf("Checking if '%s' is satisfiable...\n", formula_string);

    // Verify that the formula is in CNF.
    // TODO

    // Parse the formula and create the data structure.
    formula f = createFormula(formula_string);

    bool satisfiable = DPLL(f);

    if (satisfiable) printf("SATISFIABLE\n");
    else printf("NOT SATISFIABLE\n");

    freeFormula(f);
    return 0;
}

bool DPLL(formula f) {
    // If formula has no clauses, it is satisfiable.
    if (f->clauses == NULL) {
        printf("No clauses\n");
        return true;
    }

    // If formula has an empty clause, it is not satisfiable.
    clause c = f->clauses;
    while (c != NULL) {
        if (c->literals == NULL) {
            printf("Found empty clause\n");
            return false;
        }
        c = c->next;
    }

    // Unit propagation
    // If formula contains a clause with just one literal, you have to make it
    // true!
    c = f->clauses;
    
    while (c != NULL) {
        // Check if this is a unit clause
        literal lit = c->literals;
        if (lit != NULL && lit->next == NULL) {
            printf("Found unit clause %c\n", lit->l);
            // Perform unit propgation: make this literal true 
            f = unit_propagate(f, lit->l, lit->negation);

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

    // for every literal l that occurs pure in in formula
    //     formula <-- pure_literal_assign(formula, l)

    
    if (f->clauses == NULL || f->clauses->literals == NULL) {
        // The formula has no clauses, so it is satisfiable.
        return true;
    }
    // Splitting case, choose any literal.
    literal lit = f->clauses->literals;

    return DPLL(unit_propagate(f, lit->l, lit->negation));
    // TODO temporary return statement, deep copy f?
 
    // return DPLL(formula ^ v) or DPLL(Simplify(formula ^ ~v))
    return false;
}


// Unit propagation:
// If a clause is a unit clause (it contains only a single unassigned
// literal), this clause can only be satisfied by assigning the necessary 
// value to make this literal true.
//
// If l is a unit clause, l should be made true.
// Remove clauses in the formula where l appears and remove ~l from
// clauses where it appears.
//
// If ~l is a unit clause, l should be made false.
// Remove clauses in the formula where ~l appears and remove l from
// clauses where it appears.
formula unit_propagate(formula f, char l, int negation) {
    clause prevClause = NULL;
    clause c = f->clauses;
    while (c != NULL) {
        literal prevLit = NULL;
        literal lit = c->literals;
        bool removedClause = false;
        while (lit != NULL) {
            // Check if this literal is the one we are setting to true.
            if (lit->l == l && lit->negation == negation) {
                // Remove this entire clause, since the literal satisfies 
                // this clause.
                printf("Removing the clause with %d, %c\n", negation, l);
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
            else if (lit->l == l) {
                // Remove this literal from the clause because it can't make
                // this clause true.
                if (prevLit == NULL) {
                    c->literals = lit->next;
                } else {
                    prevLit->next = lit->next;
                }
            }
            prevLit = lit;
            lit = lit->next;
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

formula Simplify(formula f, char l) {
    // remove clauses in formula where l is positive

    // remove ~l from clauses where it appears

    // return new formula
    return f;
}

