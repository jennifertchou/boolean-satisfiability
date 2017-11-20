
#ifndef _FORMULA_H_
#define _FORMULA_H_

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
    int maxNumClauses;
    int maxNumLiterals;

};
typedef struct formula_t* formula;

void removeWhitespace(char* source);
int countNumClauses(char* s);
int countNumLiterals(char* s);
formula createFormula(char* input);
clause createClause(char* s);
void freeFormula(formula f);
void freeClause(clause c);

#endif