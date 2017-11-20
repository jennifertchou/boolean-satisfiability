
#ifndef _UTIL_H_
#define _UTIL_H_

void removeWhitespace(char* source);
formula createFormula(char* input);
clause createClause(char* s, hset literalsSet);
void freeFormula(formula f);
void freeClause(clause c);

#endif