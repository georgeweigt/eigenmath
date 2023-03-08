function
power()
{
	var h, i, n, p1, BASE, EXPO;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE) && istensor(EXPO)) {
		push_symbol(POWER);
		push(BASE);
		push(EXPO);
		list(3);
		return;
	}

	if (istensor(EXPO)) {
		p1 = copy_tensor(EXPO);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(BASE);
			push(p1.elem[i]);
			power();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (istensor(BASE)) {
		p1 = copy_tensor(BASE);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			push(EXPO);
			power();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(Math.E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(Math.PI);
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
		h = stack.length;
		push(BASE);
		factor_factor();
		n = stack.length - h;
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
			sort_factors(h);
			list(n);
			push_symbol(MULTIPLY);
			swap();
			cons();
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
		h = stack.length;
		p1 = cdr(BASE);
		while (iscons(p1)) {
			push(car(p1));
			push(EXPO);
			power();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
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
