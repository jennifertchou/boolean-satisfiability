
#include <stdbool.h>
#include "formula.h"

#ifndef _DPLL_H_
#define _DPLL_H_

bool DPLL(formula f, char assignment[]);
formula unit_propagate(formula f, int l, int negation);
formula pure_literal_assign(formula f, char l);
formula simplify(formula f, int l, int negation);

#endif
