#include "defs.h"

void
eval_simplify(void)
{
	push(cadr(p1));
	eval();
	rationalize();
	eval();
}
