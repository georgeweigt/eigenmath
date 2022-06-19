#include "defs.h"

void
eval_defint(struct atom *p1)
{
	struct atom *F, *X, *A, *B;

	push(cadr(p1));
	eval();
	F = pop();

	p1 = cddr(p1);

	while (iscons(p1)) {

		push(car(p1));
		eval();
		X = pop();

		push(cadr(p1));
		eval();
		A = pop();

		push(caddr(p1));
		eval();
		B = pop();

		push(F);
		push(X);
		integral();
		F = pop();

		push(F);
		push(X);
		push(B);
		subst();
		eval();

		push(F);
		push(X);
		push(A);
		subst();
		eval();

		subtract();
		F = pop();

		p1 = cdddr(p1);
	}

	push(F);
}
