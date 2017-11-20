#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "DPLL.h"
#include "util.h"
#include "queues.h"


void removeWhitespace(char* source) {
    char* i = source;
    char* j = source;
    while(*j != 0) {
        *i = *j++;
        if ((*i != ' ') && (*i != '(') && (*i != ')')) {
            i++;
        }
    }
    *i = 0;
}

formula createFormula(char* s) {
    removeWhitespace(s);

    formula f = malloc(sizeof(struct formula_t));

    // Save all the clauses in a queue for now in order for strtok
    // to work when parsing the clause.
    queue Q = queue_new();

    char* clause_string = strtok(s, "^");
    while (clause_string != NULL) {
        enq(Q, (void*)clause_string);
        clause_string = strtok(NULL, "^");
    }

    clause prevClause = NULL;
    while (!queue_empty(Q)) {
        char* clause_string = (char*)deq(Q);
        //printf("%s\n", clause_string);
        clause c = createClause(clause_string);
        assert(c != NULL);

        if (prevClause == NULL) {
            f->clauses = c;
            prevClause = c;
        } else {
            prevClause->next = c;
            prevClause = c; 
        }   
    }
    if (prevClause != NULL) {
        prevClause->next = NULL;
    }

    queue_free(Q, NULL);

    return f;
}

clause createClause(char* s) {
    clause c = malloc(sizeof(struct clause_t));

    char* lit_string = strtok(s, "v");

    literal prevLit = NULL;
    while (lit_string != NULL) {
        literal lit = malloc(sizeof(struct literal_t));

        if (lit_string[0] != '~') {
            lit->l = lit_string[0];
            lit->negation = 0;
        } else {
            lit->l = lit_string[1];
            lit->negation = 1;
        }

        //printf("%d,%c\n", lit->negation, lit->l);
        if (prevLit == NULL) {
            c->literals = lit;
            prevLit = lit;
        } else {
            prevLit->next = lit;
            prevLit = lit;
        }
        
        lit_string = strtok(NULL, "v");
    }
    if (prevLit != NULL) {
        prevLit->next = NULL;
    }

    return c;
}

void freeFormula(formula f) {
    clause c = f->clauses;
    while (c != NULL) {
        literal lit = c->literals;
        while (lit != NULL) {
            literal nextLit = lit->next;
            free(lit);
            lit = nextLit;
        }
        clause nextClause = c->next;
        free(c);
        c = nextClause;
    }
    free(f);
}