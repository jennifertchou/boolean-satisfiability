#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "DPLL.h"
#include "formula.h"
#include "queues.h"
#include "hset.h"


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

int countNumClauses(char* s) {
    char* s_copy = malloc(strlen(s) + 1); 
    strcpy(s_copy, s);

    int numClauses = 0;
    char* clause_string = strtok(s_copy, "^");
    while (clause_string != NULL) {
        numClauses += 1;
        clause_string = strtok(NULL, "^");
    }
    free(s_copy);
    return numClauses;
}

int countNumLiterals(char* s) {
    char* s_copy = malloc(strlen(s) + 1); 
    strcpy(s_copy, s);

    queue Q = queue_new();
    char* clause_string = strtok(s_copy, "^");
    while (clause_string != NULL) {
        enq(Q, (void*)clause_string);
        clause_string = strtok(NULL, "^");
    }

    int numLiterals = 0;
    while (!queue_empty(Q)) {
        char* clause_string = (char*)deq(Q);
        char* lit_string = strtok(clause_string, "v");
        while (lit_string != NULL) {
            numLiterals += 1;
            lit_string = strtok(NULL, "v");
        }
    }
    queue_free(Q, NULL);
    free(s_copy);
    return numLiterals;
}

formula createFormula(char* s) {
    removeWhitespace(s);
    formula f = malloc(sizeof(struct formula_t));

    f->maxNumClauses = countNumClauses(s);
    f->maxNumLiterals = countNumLiterals(s);

    // Save all the clauses in a queue for now in order for strtok
    // to work when parsing the literals in the clause.
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

void printFormula(formula f) {
    clause c = f->clauses;
    while (c != NULL) {
        printf("(");
        literal lit = c->literals;
        while (lit != NULL) {
            if (lit->negation) {
                printf("~");
            }

            char litString[2] = "\0"; /* gives {\0, \0} */
            litString[0] = lit->l;
            printf(litString);

            if (lit->next != NULL) {
                printf(" v ");
            }
            lit = lit->next;
        }
        printf(")");
        if (c->next != NULL) {
            printf(" ^ ");
        }
        c = c->next;
    }
    printf("\n");
}

formula copyFormula(formula f) {
    char s[f->maxNumLiterals * 6 + 1];
    memset(s, 0, sizeof(char) * (f->maxNumLiterals * 6 + 1));

    clause c = f->clauses;
    while (c != NULL) {
        strcat(s, "(");
        literal lit = c->literals;
        while (lit != NULL) {
            if (lit->negation) {
                strcat(s, "~");
            }

            char litString[2] = "\0"; /* gives {\0, \0} */
            litString[0] = lit->l;
            strcat(s, litString);

            if (lit->next != NULL) {
                strcat(s, " v ");
            }
            lit = lit->next;
        }
        strcat(s, ")");
        if (c->next != NULL) {
            strcat(s, " ^ ");
        }
        c = c->next;
    }
    return createFormula(s);
}

void freeFormula(formula f) {
    clause c = f->clauses;
    while (c != NULL) {
        clause nextClause = c->next;
        freeClause(c);
        c = nextClause;
    }
    free(f);
}

void freeClause(clause c) {
    literal lit = c->literals;
    while (lit != NULL) {
        literal nextLit = lit->next;
        free(lit);
        lit = nextLit;
    }
    free(c);
}
