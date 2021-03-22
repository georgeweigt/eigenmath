#include "defs.h"

void
eval_degree(void)
{
	push(cadr(p1));
	eval();
	push(caddr(p1));
	eval();
	p1 = pop();
	if (p1 == symbol(NIL))
		push_symbol(X_LOWER);
	else
		push(p1);
	degree();
}

//	Find the degree of a polynomial
//
//	Input:		tos-2		p(x)
//
//			tos-1		x
//
//	Output:		Result on stack
//
//	Note: Finds the largest numerical power of x. Does not check for
//	weirdness in p(x).

#undef POLY
#undef X
#undef YDEGREE

#define POLY p1
#define X p2
#define YDEGREE p3

void
degree(void)
{
	save();
	X = pop();
	POLY = pop();
	YDEGREE = zero;
	degree_nib(POLY);
	push(YDEGREE);
	restore();
}

void
degree_nib(struct atom *p)
{
	if (equal(p, X)) {
		if (iszero(YDEGREE))
			YDEGREE = one;
	} else if (car(p) == symbol(POWER)) {
		if (equal(cadr(p), X) && isnum(caddr(p)) && lessp(YDEGREE, caddr(p)))
			YDEGREE = caddr(p);
	} else if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			degree_nib(car(p));
			p = cdr(p);
		}
	}
}
