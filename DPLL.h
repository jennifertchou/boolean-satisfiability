
#include <stdbool.h>
#include "formula.h"

#ifndef _DPLL_H_
#define _DPLL_H_

bool DPLL_DIMACS(formula f, char assignment[]);
bool DPLL(formula f);
formula unit_propagate(formula f, char l, int intL, int negation);
formula pure_literal_assign(formula f, char l);
formula simplify(formula f, char l, int intL, int negation);

#endif
