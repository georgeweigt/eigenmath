#include "defs.h"

void
eval_for(void)
{
	int j, k;

	p1 = cdr(p1);
	p2 = car(p1);
	if (!issymbol(p2))
		stop("for: 1st arg?");

	p1 = cdr(p1);
	push(car(p1));
	eval();
	j = pop_integer();
	if (j == ERR)
		stop("for: 2nd arg?");

	p1 = cdr(p1);
	push(car(p1));
	eval();
	k = pop_integer();
	if (k == ERR)
		stop("for: 3rd arg?");

	p1 = cdr(p1);

	save_binding(p2);

	for (;;) {
		push_integer(j);
		p3 = pop();
		set_binding(p2, p3);
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

	restore_binding(p2);

	push_symbol(NIL); // return value
}
