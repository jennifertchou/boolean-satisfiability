#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "DPLL.h"
#include "formula.h"
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
    f->isDIMACS = false;

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

formula createFormulaFromDIMACS(char* fileName) {
    formula f = malloc(sizeof(struct formula_t));
    f->isDIMACS = true;
    int numVars = 0; // Will always get initialized.
    int numClauses = 0; // Will always get initialized.
    clause prevClause = NULL;

    FILE* file = fopen(fileName, "r");

    if (file == NULL) {
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments.
        if (line[0] == 'c') {
            continue;
        }
        // Find problem statement line : p cnf numVars numClauses.
        if (line[0] == 'p') {
            strtok(line, " ");
            strtok(NULL, " ");
            numVars = atoi(strtok(NULL, " "));
            numClauses = atoi(strtok(NULL, " "));
            continue;
        }

        // EOF.
        if (line[0] == '%') {
            break;
        }

        // Normal case: x y z 0.
        clause c = createClauseFromDIMACS(line);
        assert(c != NULL);

        if (prevClause == NULL) {
            f->clauses = c;
            prevClause = c;
        } else {
            prevClause->next = c;
            prevClause = c; 
        }   
    }
    
    fclose(file);

    if (prevClause != NULL) {
        prevClause->next = NULL;
    }

    f->maxNumLiterals = numVars;
    f->maxNumClauses = numClauses;
    return f;
}

// line looks like: x y z 0
clause createClauseFromDIMACS(char* line) {
    clause c = malloc(sizeof(struct clause_t));

    literal lit1 = malloc(sizeof(struct literal_t));
    literal lit2 = malloc(sizeof(struct literal_t));
    literal lit3 = malloc(sizeof(struct literal_t));

    char* l1 = strtok(line, " ");
    lit1->intL = abs(atoi(l1));
    if (l1[0] == '-') {
        lit1->negation = 1;
    } else {
        lit1->negation = 0;
    }

    char* l2 = strtok(NULL, " ");
    lit2->intL = abs(atoi(l2));
    if (l2[0] == '-') {
        lit2->negation = 1;
    } else {
        lit2->negation = 0;
    }

    char* l3 = strtok(NULL, " ");
    lit3->intL = abs(atoi(l3));
    if (l3[0] == '-') {
        lit3->negation = 1;
    } else {
        lit3->negation = 0;
    }

    c->literals = lit1;
    lit1->next = lit2;
    lit2->next = lit3;
    lit3->next = NULL;

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
            if (!f->isDIMACS) {
                char litString[2] = "\0"; /* gives {\0, \0} */
                litString[0] = lit->l;
                printf(litString);
            } else {
                printf("%d", lit->intL);
            }

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
    formula f_copy = malloc(sizeof(struct formula_t));
    f_copy->maxNumClauses = f->maxNumClauses;
    f_copy->maxNumLiterals = f->maxNumLiterals;
    f_copy->isDIMACS = f->isDIMACS;

    clause c = f->clauses;
    clause prevClause = NULL;
    while (c != NULL) {
        clause c_copy = malloc(sizeof(struct clause_t));
        if (prevClause == NULL) {
            f_copy->clauses = c_copy;
        } else {
            prevClause->next = c_copy;
        }
        prevClause = c_copy;

        literal lit = c->literals;
        literal prevLit = NULL;
        while (lit != NULL) {
            literal lit_copy = malloc(sizeof(struct literal_t));
            if (prevLit == NULL) {
                c_copy->literals = lit_copy;
            } else {
                prevLit->next = lit_copy;
            }
            prevLit = lit_copy;

            lit_copy->negation = lit->negation;
            if (!f->isDIMACS) {
                lit_copy->l = lit->l;
            } else {
                lit_copy->intL = lit->intL;
            }
            lit = lit->next;
        }
        if (prevLit != NULL) {
            prevLit->next = NULL;
        }
        c = c->next;
    }
    if (prevClause != NULL) {
        prevClause->next = NULL;
    }

    return f_copy;
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
        freeLiteral(lit);
        lit = nextLit;
    }
    free(c);
}

void freeLiteral(literal lit) {
    free(lit);
}
