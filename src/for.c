#include "defs.h"

void
eval_for(void)
{
	int j, k;

	p2 = cadr(p1);
	if (!isusersymbol(p2))
		stop("for: symbol error");

	push(caddr(p1));
	eval();
	j = pop_integer();

	push(cadddr(p1));
	eval();
	k = pop_integer();

	p1 = cddddr(p1);

	save_symbol(p2);

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_symbol(p2, p3, symbol(NIL));
		p3 = p1;
		while (iscons(p3)) {
			push(car(p3));
			eval();
			pop();
			p3 = cdr(p3);
		}
		if (j < k)
			j++;
		else if (j > k)
			j--;
		else
			break;
	}

	restore_symbol(p2);

	push_symbol(NIL); // return value
}
