void
eval_power(struct atom *p1)
{
	int t;
	struct atom *p2;

	expanding--; // undo expanding++ in eval

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

	push(p2); // push exponent

	power();

	expanding++;
}

void
power(void)
{
	int h, i, n;
	struct atom *p1, *BASE, *EXPO;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE)) {
		power_tensor(BASE, EXPO);
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
		power_numbers(BASE, EXPO);
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

	// BASE is an integer?

	if (isinteger(BASE)) {
		// raise each factor in BASE to power EXPO
		// EXPO is not numerical, that case was handled by power_numbers() above
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
			cons(); // prepend MULTIPLY to list
		}
		return;
	}

	// BASE is a numerical fraction?

	if (isfraction(BASE)) {
		// power numerator, power denominator
		// EXPO is not numerical, that case was handled by power_numbers() above
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
		power_natural_number(EXPO);
		return;
	}

	// (a + b) ^ c

	if (car(BASE) == symbol(ADD)) {
		power_sum(BASE, EXPO);
		return;
	}

	// (a b) ^ c  -->  (a ^ c) (b ^ c)

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

	// (a ^ b) ^ c  -->  a ^ (b c)

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

// BASE is a sum of terms

void
power_sum(struct atom *BASE, struct atom *EXPO)
{
	int h, i, n;
	struct atom *p1, *p2;

	if (iscomplexnumber(BASE) && isnum(EXPO)) {
		power_complex_number(BASE, EXPO);
		return;
	}

	if (expanding == 0 || !issmallinteger(EXPO) || isnegativenumber(EXPO)) {
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
power_tensor(struct atom *BASE, struct atom *EXPO)
{
	int i, n;
	struct atom *p1;

	p1 = copy_tensor(BASE);

	n = p1->u.tensor->nelem;

	for (i = 0; i < n; i++) {
		push(p1->u.tensor->elem[i]);
		push(EXPO);
		power();
		p1->u.tensor->elem[i] = pop();
	}

	push(p1);
}
