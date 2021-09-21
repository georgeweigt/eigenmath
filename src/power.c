#include "defs.h"

void
eval_power(void)
{
	int t;

	// evaluate exponent

	push(caddr(p1));
	eval();
	p2 = pop();

	// if exponent is negative then evaluate base without expanding

	push(cadr(p1));

	if (isnegativenumber(p2)) {
		t = expanding;
		expanding = 0;
		eval();
		expanding = t;
	} else
		eval();

	push(p2);

	power();
}

void
power(void)
{
	save();
	power_nib();
	restore();
}

void
power_nib(void)
{
	int h, i, n;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE)) {
		power_tensor();
		return;
	}

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(M_E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(M_PI);
		BASE = pop();
	}

	if (isnum(BASE) && isnum(EXPO)) {
		power_numbers();
		return;
	}

	// expr^0

	if (iszero(EXPO)) {
		push_integer(1);
		return;
	}

	// 0^expr

	if (iszero(BASE)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	// 1^expr

	if (isplusone(BASE)) {
		push_integer(1);
		return;
	}

	// expr^1

	if (isplusone(EXPO)) {
		push(BASE);
		return;
	}

	// BASE is an integer? (EXPO is not numerical)

	if (isinteger(BASE)) {
		// raise each factor in BASE to power EXPO
		h = tos;
		push(BASE);
		factor_factor();
		n = tos - h;
		for (i = 0; i < n; i++) {
			p1 = stack[h + i];
			if (car(p1) == symbol(POWER)) {
				push_symbol(POWER);
				push(cadr(p1)); // base
				push(caddr(p1)); // expo
				push(EXPO);
				multiply();
				list(3);
			} else {
				push_symbol(POWER);
				push(p1);
				push(EXPO);
				list(3);
			}
			stack[h + i] = pop();
		}
		if (n > 1) {
			sort_factors(n);
			list(n);
			push_symbol(MULTIPLY);
			swap();
			cons();
		}
		return;
	}

	// BASE is a numerical fraction? (EXPO is not numerical)

	if (isfraction(BASE)) {
		// power numerator, power denominator
		push(BASE);
		numerator();
		push(EXPO);
		power();
		push(BASE);
		denominator();
		push(EXPO);
		negate();
		power();
		multiply();
		return;
	}

	// BASE = e ?

	if (BASE == symbol(EXP1)) {
		power_natural_number();
		return;
	}

	// do this before checking for (a + b)^n

	if (iscomplexnumber(BASE) && isnum(EXPO)) {
		power_complex_number();
		return;
	}

	// (a + b)^n -> (a + b) * (a + b) ...

	if (car(BASE) == symbol(ADD)) {
		power_sum();
		return;
	}

	// (a * b) ^ c -> (a ^ c) * (b ^ c)

	if (car(BASE) == symbol(MULTIPLY)) {
		h = tos;
		p1 = cdr(BASE);
		while (iscons(p1)) {
			push(car(p1));
			push(EXPO);
			power();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	// (a^b)^c -> a^(b * c)

	if (car(BASE) == symbol(POWER)) {
		push(cadr(BASE));
		push(caddr(BASE));
		push(EXPO);
		multiply_expand(); // always expand products of exponents
		power();
		return;
	}

	// none of the above

	push_symbol(POWER);
	push(BASE);
	push(EXPO);
	list(3);
}

// (a + b)^n -> (a + b) * (a + b) ...

void
power_sum(void)
{
	int h, i, n;

	if (expanding == 0 || !isnum(EXPO) || isnegativenumber(EXPO) || isfraction(EXPO) || !issmallinteger(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	push(EXPO);
	n = pop_integer();

	// square the sum first (prevents infinite loop through multiply)

	h = tos;

	p1 = cdr(BASE);

	while (iscons(p1)) {
		p2 = cdr(BASE);
		while (iscons(p2)) {
			push(car(p1));
			push(car(p2));
			multiply();
			p2 = cdr(p2);
		}
		p1 = cdr(p1);
	}

	add_terms(tos - h);

	// continue up to power n

	for (i = 2; i < n; i++) {
		push(BASE);
		multiply();
	}
}

void
sqrtfunc(void)
{
	push_rational(1, 2);
	power();
}

void
power_tensor(void)
{
	int i, n;

	push(BASE);
	copy_tensor();
	p1 = pop();

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(EXPO);
		power();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
