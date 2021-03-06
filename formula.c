#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "DPLL.h"
#include "formula.h"
#include "lib/queues.h"


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

    f->maxNumLiterals = countNumLiterals(s);
    f->clauses = NULL;

    // Put all the clauses in a queue for now in order for strtok
    // to work when parsing the literals within the clause.
    queue Q = queue_new();
    char* clause_string = strtok(s, "^");
    while (clause_string != NULL) {
        enq(Q, (void*)clause_string);
        clause_string = strtok(NULL, "^");
    }

    // Start emptying the queue of clauses and parse each clause.
    clause prevClause = NULL;
    while (!queue_empty(Q)) {
        char* clause_string = (char*)deq(Q);
        clause c = createClause(clause_string, false /* isDIMACS */);
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

formula createFormulaFromDIMACS(char* fileName) {
    formula f = malloc(sizeof(struct formula_t));
    int numVars = 0; // Will always get initialized.
    int numClauses = 0; // Will always get initialized.
    int numClausesSeen = 0;
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

        // Normal case: x y z 0.
        clause c = createClause(line, true /* isDIMACS */);
        assert(c != NULL);

        if (prevClause == NULL) {
            f->clauses = c;
            prevClause = c;
        } else {
            prevClause->next = c;
            prevClause = c; 
        }   

        numClausesSeen += 1;
        if (numClausesSeen >= numClauses) {
            break;
        }
    }
    
    fclose(file);

    if (prevClause != NULL) {
        prevClause->next = NULL;
    }

    f->maxNumLiterals = numVars;
    return f;
}

clause createClause(char* s, bool isDIMACS) {
    clause c = malloc(sizeof(struct clause_t));
    c->literals = NULL;

    char* l = strtok(s, " v");

    literal prevLit = NULL;
    while (l != NULL) {
        if (isDIMACS && atoi(l) == 0) {
            break;
        }

        literal lit = malloc(sizeof(struct literal_t));

        lit->l = abs(atoi(l));
        if (l[0] == '-') {
            lit->negation = 1;
        } else {
            lit->negation = 0;
        }

        if (prevLit == NULL) {
            c->literals = lit;
            prevLit = lit;
        } else {
            prevLit->next = lit;
            prevLit = lit;
        }
        
        l = strtok(NULL, " v");
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
                printf("-");
            }
            printf("%d", lit->l);

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
    f_copy->maxNumLiterals = f->maxNumLiterals;

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

            lit_copy->l = lit->l;
           
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
