function
mag()
{
	var h, p1, RE, IM;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		abs();
		return;
	}

	// -1 to a power?

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_integer(1);
		return;
	}

	// exponential?

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		real();
		exp();
		return;
	}

	// product?

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			mag();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// sum?

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(RE);
		push(RE);
		multiply();
		push(IM);
		push(IM);
		multiply();
		add();
		push_rational(1, 2);
		power();
		return;
	}

	// real

	push(p1);
}
