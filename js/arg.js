function
arg()
{
	var i, n, t, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			arg();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	t = isdoublesomewhere(p1);

	push(p1);
	numerator();
	arg1();

	push(p1);
	denominator();
	arg1();

	subtract();

	if (t)
		floatf();
}

function
arg1()
{
	var h, p1, RE, IM;

	p1 = pop();

	if (isrational(p1)) {
		if (p1.a >= 0)
			push_integer(0);
		else {
			push_symbol(PI);
			negate();
		}
		return;
	}

	if (isdouble(p1)) {
		if (p1.d >= 0.0)
			push_double(0.0);
		else
			push_double(-Math.PI);
		return;
	}

	// (-1) ^ expr

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(PI);
		push(caddr(p1));
		multiply();
		return;
	}

	// e ^ expr

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		imag();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			arg();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar and clock forms
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(IM);
		push(RE);
		arctan();
		return;
	}

	push_integer(0); // p1 is real
}
