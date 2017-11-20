
#include <stdbool.h>
#include "hset.h"

#ifndef _DPLL_H_
#define _DPLL_H_

// --------------------------------------
// --- BOOLEAN FORMULA REPRESENTATION ---
// --------------------------------------

// A formula is a linked list of clauses.
// f->clauses = clause -> clause -> clause -> NULL
// 
// A clause is a linked list of literals.
// c->literals = literal -> literal -> NULL

struct literal_t {
    int negation;
    char l;
    struct literal_t* next;
};
typedef struct literal_t* literal;

struct clause_t {
    struct literal_t* literals;
    struct clause_t* next;
};
typedef struct clause_t* clause;

struct formula_t {
    struct clause_t* clauses;
    hset literalsSet;

};
typedef struct formula_t* formula;


bool DPLL(formula f);
formula unit_propagate(formula f, char l, int negation);
formula pure_literal_assign(formula f, char l);
formula Simplify(formula f, char l);

#endif