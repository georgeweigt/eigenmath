#include "defs.h"

void
eval_add(void)
{
	int h = tos;
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		eval();
		p1 = cdr(p1);
	}
	add_terms(tos - h);
}

void
add(void)
{
	add_terms(2);
}

void
add_terms(int n)
{
	save();
	add_terms_nib(n);
	restore();
}

void
add_terms_nib(int n)
{
	int h = tos - n;
	struct atom **s = stack + h;
	if (n == 1)
		return;
	if (n == 2) {
		p1 = s[0];
		p2 = s[1];
		if (isnum(p1) && isnum(p2)) {
			tos = h; // pop all
			add_numbers();
			return;
		}
	}
	flatten_terms(h);
	censor_terms(h);
	combine_terms(h);
	n = tos - h;
	switch (n) {
	case 0:
		push(zero);
		break;
	case 1:
		break;
	default:
		list(n);
		push_symbol(ADD);
		swap();
		cons();
		break;
	}
}

void
flatten_terms(int h)
{
	int i, n = tos - h;
	struct atom **s = stack + h;
	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (car(p1) == symbol(ADD)) {
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
censor_terms(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;
	for (i = 0; i < n; i++) {
		p1 = s[i];
		if (equaln(p1, 0)) {
			// remove term
			for (j = i + 1; j < n; j++)
				s[j - 1] = s[j];
			i--;
			n--;
			tos--;
		}
	}
}

// congruent terms are combined by adding numerical coefficients

void
combine_terms(int h)
{
	int m, n = tos - h;
	while (n > 1) {
		sort_terms(n);
		m = n;
		combine_terms_nib(h);
		n = tos - h;
		if (m == n)
			break; // did not combine any terms
	}
}

void
combine_terms_nib(int h)
{
	int i, j, n = tos - h;
	struct atom **s = stack + h;
	for (i = 0; i < n - 1; i++) {
		if (combine_adjacent_terms(s + i)) {
			if (equaln(s[i], 0)) {
				// remove 2 terms
				for (j = i + 2; j < n; j++)
					s[j - 2] = s[j];
				n -= 2;
				tos -= 2;
			} else {
				// remove 1 term
				for (j = i + 2; j < n; j++)
					s[j - 1] = s[j];
				n--;
				tos--;
			}
			i--; // use the same index again
		}
	}
}

int
combine_adjacent_terms(struct atom **s)
{
	int state;

	p1 = s[0];
	p2 = s[1];

	if (istensor(p1) && istensor(p2)) {
		push(p1);
		push(p2);
		tensor_plus_tensor();
		p1 = pop();
		if (p1 == symbol(NIL))
			return 0;
		s[0] = p1;
		return 1; // added tensors
	}

	if (istensor(p1) || istensor(p2))
		return 0; // cannot add tensor and something else

	if (isnum(p1) && isnum(p2)) {
		add_numbers();
		s[0] = pop();
		return 1; // added numbers
	}

	if (isnum(p1) || isnum(p2))
		return 0; // cannot add number and something else

	p3 = p1;
	p4 = p2;

	p1 = one; // coeff of 1st term
	p2 = one; // coeff of 2nd term

	state = 0;

	if (car(p3) == symbol(MULTIPLY)) {
		p3 = cdr(p3);
		state = 1; // p3 is now an improper expr
		if (isnum(car(p3))) {
			p1 = car(p3);
			p3 = cdr(p3);
			if (cdr(p3) == symbol(NIL)) {
				p3 = car(p3);
				state = 0; // p3 is now a proper expr
			}
		}
	}

	if (car(p4) == symbol(MULTIPLY)) {
		p4 = cdr(p4);
		if (isnum(car(p4))) {
			p2 = car(p4);
			p4 = cdr(p4);
			if (cdr(p4) == symbol(NIL))
				p4 = car(p4);
		}
	}

	if (!equal(p3, p4))
		return 0; // terms differ, cannot add

	add_numbers(); // add coeffs p1 and p2

	if (state) {
		push_symbol(MULTIPLY);
		push(p3);
		cons();
	} else
		push(p3);

	multiply();

	s[0] = pop();

	return 1;
}

void
sort_terms(int n)
{
	qsort(stack + tos - n, n, sizeof (struct atom *), sort_terms_func);
}

int
sort_terms_func(const void *q1, const void *q2)
{
	return cmp_terms(*((struct atom **) q1), *((struct atom **) q2));
}

int
cmp_terms(struct atom *p1, struct atom *p2)
{
	int a, b, c;

	// 1st level: imaginary terms on the right

	a = is_imaginary_term(p1);
	b = is_imaginary_term(p2);

	if (a == 0 && b == 1)
		return -1; // ok

	if (a == 1 && b == 0)
		return 1; // out of order

	// 2nd level: numericals on the right

	if (isnum(p1) && isnum(p2))
		return 0; // don't care about order, save time, don't compare

	if (isnum(p1))
		return 1; // out of order

	if (isnum(p2))
		return -1; // ok

	// 3rd level: sort by factors

	a = 0;
	b = 0;

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		a = 1; // p1 is a list of factors
		if (isnum(car(p1))) {
			// skip over coeff
			p1 = cdr(p1);
			if (cdr(p1) == symbol(NIL)) {
				p1 = car(p1);
				a = 0;
			}
		}
	}

	if (car(p2) == symbol(MULTIPLY)) {
		p2 = cdr(p2);
		b = 1; // p2 is a list of factors
		if (isnum(car(p2))) {
			// skip over coeff
			p2 = cdr(p2);
			if (cdr(p2) == symbol(NIL)) {
				p2 = car(p2);
				b = 0;
			}
		}
	}

	if (a == 0 && b == 0)
		return cmp_factors(p1, p2);

	if (a == 0 && b == 1) {
		c = cmp_factors(p1, car(p2));
		if (c == 0)
			c = -1; // length(p1) < length(p2)
		return c;
	}

	if (a == 1 && b == 0) {
		c = cmp_factors(car(p1), p2);
		if (c == 0)
			c = 1; // length(p1) > length(p2)
		return c;
	}

	while (iscons(p1) && iscons(p2)) {
		c = cmp_factors(car(p1), car(p2));
		if (c)
			return c;
		p1 = cdr(p1);
		p2 = cdr(p2);
	}

	if (iscons(p1))
		return 1; // length(p1) > length(p2)

	if (iscons(p2))
		return -1; // length(p1) < length(p2)

	return 0;
}

int
is_imaginary_term(struct atom *p)
{
	if (car(p) == symbol(POWER) && equaln(cadr(p), -1))
		return 1;
	if (iscons(p)) {
		p = cdr(p);
		while (iscons(p)) {
			if (caar(p) == symbol(POWER) && equaln(cadar(p), -1))
				return 1;
			p = cdr(p);
		}
	}
	return 0;
}

void
add_numbers(void)
{
	double d1, d2;

	if (p1->k == RATIONAL && p2->k == RATIONAL) {
		add_rationals();
		return;
	}

	push(p1);
	d1 = pop_double();

	push(p2);
	d2 = pop_double();

	push_double(d1 + d2);
}

void
add_rationals(void)
{
	int sign;
	uint32_t *a, *ab, *b, *ba, *c;

	if (equaln(p1, 0)) {
		push(p2);
		return;
	}

	if (equaln(p2, 0)) {
		push(p1);
		return;
	}

	ab = mmul(p1->u.q.a, p2->u.q.b);
	ba = mmul(p1->u.q.b, p2->u.q.a);

	if (p1->sign == p2->sign) {
		a = madd(ab, ba);
		sign = p1->sign;
	} else {
		switch (mcmp(ab, ba)) {
		case 1:
			a = msub(ab, ba);
			sign = p1->sign;
			break;
		case 0:
			push(zero);
			mfree(ab);
			mfree(ba);
			return;
		case -1:
			a = msub(ba, ab);
			sign = p2->sign;
			break;
		}
	}

	mfree(ab);
	mfree(ba);

	b = mmul(p1->u.q.b, p2->u.q.b);
	c = mgcd(a, b);

	push_rational_number(sign, mdiv(a, c), mdiv(b, c));

	mfree(a);
	mfree(b);
	mfree(c);
}

void
subtract(void)
{
	negate();
	add();
}
