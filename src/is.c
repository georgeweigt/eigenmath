#include "defs.h"

int
iszero(struct atom *p)
{
	int i;
	if (isrational(p))
		return MZERO(p->u.q.a);
	else if (isdouble(p))
		return p->u.d == 0.0;
	else if (istensor(p)) {
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (!iszero(p->u.tensor->elem[i]))
				return 0;
		return 1;
	} else
		return 0;
}

int
isplusone(struct atom *p)
{
	return equaln(p, 1);
}

int
isminusone(struct atom *p)
{
	return equaln(p, -1);
}

int
isinteger(struct atom *p)
{
	return isrational(p) && MEQUAL(p->u.q.b, 1);
}

int
isfraction(struct atom *p)
{
	return isrational(p) && !MEQUAL(p->u.q.b, 1);
}

int
isposint(struct atom *p)
{
	return isinteger(p) && p->sign == MPLUS;
}

int
iseveninteger(struct atom *p)
{
	return isinteger(p) && (p->u.q.a[0] & 1) == 0;
}

int
isradical(struct atom *p)
{
	return car(p) == symbol(POWER) && isposint(cadr(p)) && isfraction(caddr(p));
}

int
isnegative(struct atom *p)
{
	return isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)));
}

int
isnegativeterm(struct atom *p)
{
	return isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p)));
}

int
isnegativenumber(struct atom *p)
{
	if (isrational(p))
		return p->sign == MMINUS;
	else if (isdouble(p))
		return p->u.d < 0.0;
	else
		return 0;
}

int
iscomplexnumber(struct atom *p)
{
	return isimaginarynumber(p) || (length(p) == 3 && car(p) == symbol(ADD) && isnum(cadr(p)) && isimaginarynumber(caddr(p)));
}

int
isimaginarynumber(struct atom *p)
{
	return isimaginaryunit(p) || (length(p) == 3 && car(p) == symbol(MULTIPLY) && isnum(cadr(p)) && isimaginaryunit(caddr(p)));
}

int
isimaginaryunit(struct atom *p)
{
	return car(p) == symbol(POWER) && equaln(cadr(p), -1) && equalq(caddr(p), 1, 2);
}

// p == 1/sqrt(2) ?

int
isoneoversqrttwo(struct atom *p)
{
	return car(p) == symbol(POWER) && equaln(cadr(p), 2) && equalq(caddr(p), -1, 2);
}

// p == -1/sqrt(2) ?

int
isminusoneoversqrttwo(struct atom *p)
{
	return length(p) == 3 && car(p) == symbol(MULTIPLY) && equaln(cadr(p), -1) && isoneoversqrttwo(caddr(p));
}

// x + y * (-1)^(1/2) where x and y are double?

int
isdoublez(struct atom *p)
{
	if (car(p) == symbol(ADD)) {

		if (length(p) != 3)
			return 0;

		if (!isdouble(cadr(p))) // x
			return 0;

		p = caddr(p);
	}

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (length(p) != 3)
		return 0;

	if (!isdouble(cadr(p))) // y
		return 0;

	p = caddr(p);

	if (car(p) != symbol(POWER))
		return 0;

	if (!equaln(cadr(p), -1))
		return 0;

	if (!equalq(caddr(p), 1, 2))
		return 0;

	return 1;
}

int
ispoly(struct atom *p, struct atom *x)
{
	if (find(p, x))
		return ispoly_expr(p, x);
	else
		return 0;
}

int
ispoly_expr(struct atom *p, struct atom *x)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (!ispoly_term(car(p), x))
				return 0;
			p = cdr(p);
		}
		return 1;
	} else
		return ispoly_term(p, x);
}

int
ispoly_term(struct atom *p, struct atom *x)
{
	if (car(p) == symbol(MULTIPLY)) {
		p = cdr(p);
		while (iscons(p)) {
			if (!ispoly_factor(car(p), x))
				return 0;
			p = cdr(p);
		}
		return 1;
	} else
		return ispoly_factor(p, x);
}

int
ispoly_factor(struct atom *p, struct atom *x)
{
	if (equal(p, x))
		return 1;
	if (car(p) == symbol(POWER) && equal(cadr(p), x)) {
		if (isposint(caddr(p)))
			return 1;
		else
			return 0;
	}
	if (find(p, x))
		return 0;
	else
		return 1;
}
