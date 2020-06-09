#include "defs.h"

#undef TFACT
#undef BASE1
#undef BASE2
#undef EXPO1
#undef EXPO2
#undef COEF
#undef NUMER
#undef DENOM

#define TFACT p0
#define BASE1 p3
#define BASE2 p4
#define EXPO1 p5
#define EXPO2 p6
#define COEF p7
#define NUMER p8
#define DENOM p9

void
eval_multiply(void)
{
	int h = tos;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}
	multiply_factors(tos - h);
}

void
multiply(void)
{
	multiply_factors(2);
}

void
multiply_factors(int n)
{
	save();
	multiply_factors_nib(n);
	restore();
}

void
multiply_factors_nib(int n)
{
	int h = tos - n;
	level_factors(h);
	partition_tensor_factor(h);
	COEF = one;
	collect_numerical_factors(h);
	combine_factors(h);
	normalize_power_factors(h);
	collect_numerical_factors(h);
	reduce_radical_factors(h);
	if (isdouble(COEF) || !isplusone(COEF))
		push(COEF);
	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack
	n = tos - h;
	switch (n) {
	case 0:
		push_integer(1);
		break;
	case 1:
		break;
	default:
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
		break;
	}
	if (istensor(TFACT)) {
		push(TFACT);
		inner();
	}
}

void
level_factors(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h;
	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (car(p1) == symbol(MULTIPLY)) {
			s[i] = cadr(p1);
			p1 = cddr(p1);
			while (iscons(p1)) {
				push(car(p1));
				p1 = cdr(p1);
			}
		}
	}
}

void
partition_tensor_factor(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;
	TFACT = symbol(NIL);
	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (istensor(p1)) {
			if (istensor(TFACT))
				stop("tensor * tensor not supported, use dot or outer instead");
			TFACT = p1;
			// remove the factor
			for (j = i + 1; j < n; j++)
				s[j - 1] = s[j];
			i--;
			n--;
			tos--;
		}
	}
}

void
collect_numerical_factors(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;
	p1 = COEF;
	for (i = 0; i < n; i++) {
		p2 = s[i];
		if (isnum(p2)) {
			multiply_numbers();
			p1 = pop();
			// remove the factor
			for (j = i + 1; j < n; j++)
				s[j - 1] = s[j];
			i--;
			n--;
			tos--;
		}
	}
	COEF = p1;
	if (iszero(COEF))
		tos = h; // pop all
}

// factors that have the same base are combined by adding exponents

void
combine_factors(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;
	sort_factors_provisional(n);
	for (i = 0; i < n - 1; i++) {
		if (combine_adjacent_factors(s + i)) {
			// remove the factor
			for (j = i + 2; j < n; j++)
				s[j - 1] = s[j];
			i--;
			n--;
			tos--;
		}
	}
}

void
sort_factors_provisional(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_factors_provisional_func);
}

int
sort_factors_provisional_func(const void *q1, const void *q2)
{
	return cmp_factors_provisional(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors_provisional(struct atom *p1, struct atom *p2)
{
	if (car(p1) == symbol(POWER))
		p1 = cadr(p1); // p1 = base

	if (car(p2) == symbol(POWER))
		p2 = cadr(p2); // p2 = base

	return cmp_expr(p1, p2);
}

int
combine_adjacent_factors(struct atom **s)
{
	p1 = s[0];
	p2 = s[1];

	if (car(p1) == symbol(POWER)) {
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
	} else {
		BASE1 = p1;
		EXPO1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		BASE2 = cadr(p2);
		EXPO2 = caddr(p2);
	} else {
		BASE2 = p2;
		EXPO2 = one;
	}

	if (!equal(BASE1, BASE2))
		return 0;

	if (isdouble(BASE2))
		BASE1 = BASE2; // if mixed rational and double, use double

	push_symbol(POWER);
	push(BASE1);
	push(EXPO1);
	push(EXPO2);
	add();
	list(3);

	s[0] = pop();

	return 1;
}

void
factor_factors_maybe(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h;

	// is there at least one power with a numerical base?

	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (car(p1) == symbol(POWER) && isnum(cadr(p1)) && !isminusone(cadr(p1)))
			break;
	}

	if (i == n)
		return; // no

	// factor factors

	for (i = 0; i < n; i++) {
		push(s[i]);
		factor_factor();
		s[i] = pop(); // trick: fill hole with one of the factors
	}
}

void
normalize_power_factors(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h;
	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (car(p1) == symbol(POWER)) {
			push(cadr(p1));
			push(caddr(p1));
			power();
			p1 = pop();
			if (car(p1) == symbol(MULTIPLY)) {
				s[i] = cadr(p1);
				p1 = cddr(p1);
				while (iscons(p1)) {
					push(car(p1));
					p1 = cdr(p1);
				}
			} else
				s[i] = p1;
		}
	}
}

void
expand_sum_factors(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;

	if (n < 2)
		return;

	// search for a sum factor

	for (i = 0; i < n; i++) {
		p2 = s[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == n)
		return; // no sum factors

	// remove the sum factor

	for (j = i + 1; j < n; j++)
		s[j - 1] = s[j];

	n--;
	tos--;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum factor

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(tos - h);
}

void
sort_factors(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_factors_func);
}

int
sort_factors_func(const void *q1, const void *q2)
{
	return cmp_factors(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_factors(struct atom *p1, struct atom *p2)
{
	int a, b, c;
	struct atom *base1, *base2, *expo1, *expo2;

	a = order_factor(p1);
	b = order_factor(p2);

	if (a < b)
		return -1;

	if (a > b)
		return 1;

	if (car(p1) == symbol(POWER)) {
		base1 = cadr(p1);
		expo1 = caddr(p1);
	} else {
		base1 = p1;
		expo1 = one;
	}

	if (car(p2) == symbol(POWER)) {
		base2 = cadr(p2);
		expo2 = caddr(p2);
	} else {
		base2 = p2;
		expo2 = one;
	}

	c = cmp_expr(base1, base2);

	if (c == 0)
		c = cmp_expr(expo2, expo1); // swapped to reverse sort order

	return c;
}

//  1	number
//  2	number to power (root)
//  3	-1 to power (imaginary)
//  4	other factor (symbol, power, func, etc)
//  5	exponential
//  6	derivative

int
order_factor(struct atom *p)
{
	if (p->k == RATIONAL || p->k == DOUBLE)
		return 1;

	if (p == symbol(EXP1))
		return 5;

	if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
		return 6;

	if (car(p) == symbol(POWER)) {

		p = cadr(p); // p = base

		if (equaln(p, -1))
			return 3;

		if (p->k == RATIONAL || p->k == DOUBLE)
			return 2;

		if (p == symbol(EXP1))
			return 5;

		if (car(p) == symbol(DERIVATIVE) || car(p) == symbol(SYMBOL_D))
			return 6;
	}

	return 4;
}

// multiply numbers p1 and p2

void
multiply_numbers(void)
{
	double d1, d2;

	if (p1->k == RATIONAL && p2->k == RATIONAL) {
		multiply_rationals();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 * d2);
}

void
multiply_rationals(void)
{
	int sign;
	uint32_t *a, *b, *c;

	if (equaln(p1, 0) || equaln(p2, 0)) {
		push_integer(0);
		return;
	}

	a = mmul(p1->u.q.a, p2->u.q.a);
	b = mmul(p1->u.q.b, p2->u.q.b);

	c = mgcd(a, b);

	if (p1->sign == p2->sign)
		sign = MPLUS;
	else
		sign = MMINUS;

	push_rational_number(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

void
multiply_noexpand(void)
{
	int t = expanding;
	expanding = 0;
	multiply();
	expanding = t;
}

void
multiply_factors_noexpand(int n)
{
	int t = expanding;
	expanding = 0;
	multiply_factors(n);
	expanding = t;
}

void
negate(void)
{
	save();
	negate_nib();
	restore();
}

void
negate_nib(void)
{
	p1 = pop();

	if (isnum(p1)) {
		negate_number();
		return;
	}

	push_integer(-1);
	push(p1);
	multiply();
}

// negate p1

void
negate_number(void)
{
	uint32_t *a, *b;
	double d;

	if (iszero(p1)) {
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		d = p1->u.d;
		push_double(-1.0 * d);
		return;
	}

	a = mcopy(p1->u.q.a);
	b = mcopy(p1->u.q.b);

	if (p1->sign == MMINUS)
		push_rational_number(MPLUS, a, b);
	else
		push_rational_number(MMINUS, a, b);
}

void
negate_noexpand(void)
{
	int t = expanding;
	expanding = 0;
	negate();
	expanding = t;
}

void
reciprocate(void)
{
	save();
	reciprocate_nib();
	restore();
}

void
reciprocate_nib(void)
{
	p1 = pop();

	if (isnum(p1)) {
		reciprocate_number();
		return;
	}

	push(p1);
	push_integer(-1);
	power();
}

// 1 over p1

void
reciprocate_number(void)
{
	uint32_t *a, *b;
	double d;

	if (iszero(p1))
		stop("divide by zero");

	if (isdouble(p1)) {
		d = p1->u.d;
		push_double(1.0 / d);
		return;
	}

	a = mcopy(p1->u.q.a);
	b = mcopy(p1->u.q.b);

	push_rational_number(p1->sign, b, a);
}

void
divide(void)
{
	save();
	divide_nib();
	restore();
}

void
divide_nib(void)
{
	p2 = pop();
	p1 = pop();

	if (isnum(p1) && isnum(p2)) {
		divide_numbers();
		return;
	}

	push(p1);
	push(p2);
	push_integer(-1);
	power();
	multiply();
}

// p1 over p2

void
divide_numbers(void)
{
	double d1, d2;

	if (isrational(p1) && isrational(p2)) {
		divide_rationals();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	if (d2 == 0.0)
		stop("divide by zero");

	push_double(d1 / d2);
}

// p1 over p2

void
divide_rationals(void)
{
	uint32_t *a, *b;

	if (equaln(p2, 0))
		stop("divide by zero");

	a = mcopy(p2->u.q.a);
	b = mcopy(p2->u.q.b);

	push_rational_number(p2->sign, b, a);

	p2 = pop();

	multiply_rationals(); // uses p1 and p2
}

void
reduce_radical_factors(int h)
{
	int i, j, k = 0, n = tos - h;
	double a, b, c;
	struct atom **s = stack + h;

	if (iszero(COEF))
		return;

	if (isdouble(COEF)) {
		c = COEF->u.d;
		for (i = 0; i < n; i++) {
			p1 = s[i];
			if (isradical(p1)) {
				k++;
				push(cadr(p1)); // base
				a = pop_double();
				push(caddr(p1)); // exponent
				b = pop_double();
				c = c * pow(a, b); // a > 0 by isradical above
				// remove the factor
				for (j = i + 1; j < n; j++)
					s[j - 1] = s[j];
				i--;
				n--;
				tos--;
			}
		}
		if (k) {
			push_double(c);
			COEF = pop();
		}
		return;
	}

	if (isplusone(COEF) || isminusone(COEF))
		return; // COEF has no factors, no cancellation is possible

	for (i = 0; i < n; i++)
		if (isradical(s[i]))
			break;

	if (i == n)
		return; // no radicals

	push(COEF);
	absval();
	p1 = pop();

	push(p1);
	numerator();
	NUMER = pop();

	push(p1);
	denominator();
	DENOM = pop();

	for (; i < n; i++) {
		p1 = s[i];
		if (!isradical(p1))
			continue;
		BASE1 = cadr(p1);
		EXPO1 = caddr(p1);
		if (EXPO1->sign == MMINUS) {
			push(NUMER);
			push(BASE1);
			mod();
			p2 = pop();
			if (iszero(p2)) {
				push(NUMER);
				push(BASE1);
				divide();
				NUMER = pop();
				push_symbol(POWER);
				push(BASE1);
				push_integer(1);
				push(EXPO1);
				add();
				list(3);
				s[i] = pop();
				k++;
			}
		} else {
			push(DENOM);
			push(BASE1);
			mod();
			p2 = pop();
			if (iszero(p2)) {
				push(DENOM);
				push(BASE1);
				divide();
				DENOM = pop();
				push_symbol(POWER);
				push(BASE1);
				push_integer(-1);
				push(EXPO1);
				add();
				list(3);
				s[i] = pop();
				k++;
			}
		}
	}

	if (k) {
		push(NUMER);
		push(DENOM);
		divide();
		if (COEF->sign == MMINUS)
			negate();
		COEF = pop();
	}
}

int
isradical(struct atom *p)
{
	if (car(p) == symbol(POWER)
	&& isposint(cadr(p)) // base
	&& isrational(caddr(p))) // exponent
		return 1;
	else
		return 0;
}
