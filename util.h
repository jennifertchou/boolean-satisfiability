
#ifndef _UTIL_H_
#define _UTIL_H_

void removeWhitespace(char* source);
formula createFormula(char* input);
clause createClause(char* s);
void freeFormula(formula f);

#endif