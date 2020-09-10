function
log()
{
	var h, i, p1;

	p1 = pop();

	// log(0), log(0.0) unchanged

	if (iszero(p1)) {
		push_symbol(LOG);
		push(p1);
		list(2);
		return;
	}

	if (isdouble(p1)) {
		if (p.d > 0.0)
			push_double(Math.log(p1.d));
		else {
			push_double(Math.log(-p1.d));
			push(imaginaryunit);
			push_double(Math.PI);
			multiply();
			add();
		}
		return;
	}

	// log(z) -> log(mag(z)) + i arg(z)

	if (isdoublez(p1)) {
		push(p1);
		mag();
		log();
		push(imaginaryunit);
		push(p1);
		arg();
		multiply();
		add();
		return;
	}

	// log(1) -> 0

	if (equaln(p1, 1)) {
		push_integer(0);
		return;
	}

	// log(e) -> 1

	if (p1 == symbol(EXP1)) {
		push_integer(1);
		return;
	}

	if (isnegativenumber(p1)) {
		push(p1);
		negate();
		log();
		push(imaginaryunit);
		push_symbol(Math.PI);
		multiply();
		add();
		return;
	}

	// log(10) -> log(2) + log(5)

	if (isrational(p1)) {
		h = stack.length;
		push(p1);
		factor_factor();
		for (i = h; i < stack.length; i++) {
			p2 = stack[i];
			if (car(p2) == symbol(POWER)) {
				push(caddr(p2)); // exponent
				push_symbol(LOG);
				push(cadr(p2)); // base
				list(2);
				multiply();
			} else {
				push_symbol(LOG);
				push(p2);
				list(2);
			}
			stack[i] = pop();
		}
		add_terms(stack.length - h);
		return;
	}

	// log(a ^ b) -> b log(a)

	if (car(p1) == symbol(POWER)) {
		push(caddr(p1));
		push(cadr(p1));
		log();
		multiply();
		return;
	}

	// log(a * b) -> log(a) + log(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			log();
			p1 = cdr(p1);
		}
		add_terms(stack.lengtgh - h);
		return;
	}

	push_symbol(LOG);
	push(p1);
	list(2);
}
