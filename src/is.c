#include "defs.h"

int
iszero(struct atom *p)
{
	int i;
	switch (p->k) {
	case RATIONAL:
		if (MZERO(p->u.q.a))
			return 1;
		break;
	case DOUBLE:
		if (p->u.d == 0.0)
			return 1;
		break;
	case TENSOR:
		for (i = 0; i < p->u.tensor->nelem; i++)
			if (!iszero(p->u.tensor->elem[i]))
				return 0;
		return 1;
	default:
		break;
	}
	return 0;
}

int
isnegativenumber(struct atom *p)
{
	switch (p->k) {
	case RATIONAL:
		if (p->sign == MMINUS)
			return 1;
		break;
	case DOUBLE:
		if (p->u.d < 0.0)
			return 1;
		break;
	default:
		break;
	}
	return 0;
}

int
isplusone(struct atom *p)
{
	switch (p->k) {
	case RATIONAL:
		if (p->sign == MPLUS && MEQUAL(p->u.q.a, 1) && MEQUAL(p->u.q.b, 1))
			return 1;
		break;
	case DOUBLE:
		if (p->u.d == 1.0)
			return 1;
		break;
	default:
		break;
	}
	return 0;
}

int
isminusone(struct atom *p)
{
	switch (p->k) {
	case RATIONAL:
		if (p->sign == MMINUS && MEQUAL(p->u.q.a, 1) && MEQUAL(p->u.q.b, 1))
			return 1;
		break;
	case DOUBLE:
		if (p->u.d == -1.0)
			return 1;
		break;
	default:
		break;
	}
	return 0;
}

int
isinteger(struct atom *p)
{
	if (p->k == RATIONAL && MEQUAL(p->u.q.b, 1))
		return 1;
	else
		return 0;
}

int
isposint(struct atom *p)
{
	if (isinteger(p) && p->sign == MPLUS)
		return 1;
	else
		return 0;
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

int
isnegativeterm(struct atom *p)
{
	if (isnegativenumber(p) || (car(p) == symbol(MULTIPLY) && isnegativenumber(cadr(p))))
		return 1;
	else
		return 0;
}

int
isimaginarynumber(struct atom *p)
{
	if ((car(p) == symbol(MULTIPLY)
	&& length(p) == 3
	&& isnum(cadr(p))
	&& equal(caddr(p), imaginaryunit))
	|| equal(p, imaginaryunit))
		return 1;
	else

		return 0;
}

int
iscomplexnumber(struct atom *p)
{
	if ((car(p) == symbol(ADD)
	&& length(p) == 3
	&& isnum(cadr(p))
	&& isimaginarynumber(caddr(p)))
	|| isimaginarynumber(p))
		return 1;
	else
		return 0;
}

int
iseveninteger(struct atom *p)
{
	if (isinteger(p) && (p->u.q.a[0] & 1) == 0)
		return 1;
	else
		return 0;
}

int
isnegative(struct atom *p)
{
	if (car(p) == symbol(ADD) && isnegativeterm(cadr(p)))
		return 1;
	else if (isnegativeterm(p))
		return 1;
	else
		return 0;
}

// returns 1 if there's a symbol somewhere

int
issymbolic(struct atom *p)
{
	if (issymbol(p))
		return 1;
	else {
		while (iscons(p)) {
			if (issymbolic(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}
}

// i.e. 2, 2^3, etc.

int
isintegerfactor(struct atom *p)
{
	if (isinteger(p) || (car(p) == symbol(POWER)
	&& isinteger(cadr(p))
	&& isinteger(caddr(p))))
		return 1;
	else
		return 0;
}

int
isoneover(struct atom *p)
{
	if (car(p) == symbol(POWER)
	&& isminusone(caddr(p)))
		return 1;
	else
		return 0;
}

int
isfraction(struct atom *p)
{
	if (p->k == RATIONAL && !MEQUAL(p->u.q.b, 1))
		return 1;
	else
		return 0;
}

int
equaln(struct atom *p, int n)
{
	switch (p->k) {
	case RATIONAL:
		if (((p->sign == MMINUS && n < 0) || (p->sign == MPLUS && n >= 0))
		&& MEQUAL(p->u.q.a, abs(n)) && MEQUAL(p->u.q.b, 1))
			return 1;
		break;
	case DOUBLE:
		if (p->u.d == (double) n)
			return 1;
		break;
	default:
		break;
	}
	return 0;
}

int
equalq(struct atom *p, int a, int b)
{
	switch (p->k) {
	case RATIONAL:
		if (((p->sign == MMINUS && a < 0) || (p->sign == MPLUS && a >= 0))
		&& MEQUAL(p->u.q.a, abs(a)) && MEQUAL(p->u.q.b, b))
			return 1;
		break;
	case DOUBLE:
		if (p->u.d == (double) a / b)
			return 1;
		break;
	default:
		break;
	}
	return 0;
}

// p == 1/sqrt(2) ?

int
isoneoversqrttwo(struct atom *p)
{
	if (car(p) == symbol(POWER)
	&& equaln(cadr(p), 2)
	&& equalq(caddr(p), -1, 2))
		return 1;
	else
		return 0;
}

// p == -1/sqrt(2) ?

int
isminusoneoversqrttwo(struct atom *p)
{
	if (car(p) == symbol(MULTIPLY)
	&& equaln(cadr(p), -1)
	&& isoneoversqrttwo(caddr(p))
	&& length(p) == 3)
		return 1;
	else
		return 0;
}

int
isimaginaryunit(struct atom *p)
{
	if (car(p) == symbol(POWER) && equaln(cadr(p), -1) && equalq(caddr(p), 1, 2))
		return 1;
	else
		return 0;
}

// n/2 * i * pi ?
// return value:
//	0	no
//	1	1
//	2	-1
//	3	i
//	4	-i

int
isquarterturn(struct atom *p)
{
	int n, minussign = 0;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (equal(cadr(p), imaginaryunit)) {
		// no leading numerical factor
		if (caddr(p) != symbol(PI))
			return 0;
		if (length(p) != 3)
			return 0;
		return 2;
	}

	if (!isnum(cadr(p)))
		return 0;

	if (!equal(caddr(p), imaginaryunit))
		return 0;

	if (cadddr(p) != symbol(PI))
		return 0;

	if (length(p) != 4)
		return 0;

	push(cadr(p));
	push_integer(2);
	multiply();

	n = pop_integer();

	if (n == ERR)
		return 0;

	if (n < 1) {
		minussign = 1;
		n = -n;
	}

	switch (n % 4) {
	case 0:
		n = 1;
		break;
	case 1:
		if (minussign)
			n = 4;
		else
			n = 3;
		break;
	case 2:
		n = 2;
		break;
	case 3:
		if (minussign)
			n = 3;
		else
			n = 4;
		break;
	}

	return n;
}

// special multiple of pi?

// returns for the following multiples of pi...

//	-4/2	-3/2	-2/2	-1/2	1/2	2/2	3/2	4/2

//	4	1	2	3	1	2	3	4

int
isnpi(struct atom *p)
{
	int n;
	if (p == symbol(PI))
		return 2;
	if (car(p) == symbol(MULTIPLY)
	&& isnum(cadr(p))
	&& caddr(p) == symbol(PI)
	&& length(p) == 3)
		;
	else
		return 0;
	push(cadr(p));
	push_integer(2);
	multiply();
	n = pop_integer();
	if (n == ERR)
		return 0;
	if (n < 0)
		n = 4 - (-n) % 4;
	else
		n = 1 + (n - 1) % 4;
	return n;
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
