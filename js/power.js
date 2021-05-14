function
power()
{
	var h, p3, BASE, EXPO;

	EXPO = pop();
	BASE = pop();

	if (istensor(BASE)) {
		power_tensor();
		return;
	}

	if (iszero(BASE) && isnegativenumber(EXPO))
		stopf("divide by zero");

	if (BASE == symbol(EXP1) && isdouble(EXPO)) {
		push_double(Math.E);
		BASE = pop();
	}

	if (BASE == symbol(PI) && isdouble(EXPO)) {
		push_double(Math.PI);
		BASE = pop();
	}

	if (power_precheck(BASE, EXPO))
		return;

	// BASE and EXPO numerical?

	if (isnum(BASE) && isnum(EXPO)) {
		power_numbers(BASE, EXPO);
		return;
	}

	// BASE is a fraction?

	if (isfraction(BASE)) {
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

	// do this before checking for (a + b)^n

	if (iscomplexnumber(BASE) && isnum(EXPO)) {
		power_complex_number(BASE, EXPO);
		return;
	}

	// (a + b)^n -> (a + b) * (a + b) ...

	if (car(BASE) == symbol(ADD)) {
		power_sum(BASE, EXPO);
		return;
	}

	// (a * b) ^ c -> (a ^ c) * (b ^ c)

	if (car(BASE) == symbol(MULTIPLY)) {
		h = stack.length;
		p3 = cdr(BASE);
		while (iscons(p3)) {
			push(car(p3));
			push(EXPO);
			power();
			p3 = cdr(p3);
		}
		multiply_factors(stack.length - h);
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
