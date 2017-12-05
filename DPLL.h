
#include <stdbool.h>
#include "formula.h"

#ifndef _DPLL_H_
#define _DPLL_H_

bool DPLL(formula f, char assignment[]);
formula simplify(formula f, int l, int negation);

#endif
