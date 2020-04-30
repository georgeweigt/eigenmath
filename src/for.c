// 'for' function

#include "defs.h"

#undef I
#undef X

#define I p5
#define X p6

void
eval_for(void)
{
	int i, j, k;

	// 1st arg (quoted)

	X = cadr(p1);
	if (!issymbol(X))
		stop("for: 1st arg?");

	// 2nd arg

	push(caddr(p1));
	eval();
	j = pop_integer();
	if (j == ERR)
		stop("for: 2nd arg?");

	// 3rd arg

	push(cadddr(p1));
	eval();
	k = pop_integer();
	if (k == ERR)
		stop("for: 3rd arg?");

	// remaining args

	p1 = cddddr(p1);

	save_binding(X);

	for (i = j; i <= k; i++) {
		push_integer(i);
		I = pop();
		set_binding(X, I);
		p2 = p1;
		while (iscons(p2)) {
			push(car(p2));
			eval();
			pop();
			p2 = cdr(p2);
		}
	}

	restore_binding(X);

	// return value

	push_symbol(NIL);
}
