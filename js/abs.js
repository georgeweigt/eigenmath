function
abs()
{
	var h, p1;

	p1 = pop();

	if (istensor(p1)) {
		if (p1.dim.length > 1) {
			push_symbol(ABS);
			push(p1);
			list(2);
			return;
		}
		push(p1);
		push(p1);
		conj();
		inner();
		push_rational(1, 2);
		power();
		return;
	}

	if (isnum(p1)) {
		push(p1);
		if (isnegativenumber(p1))
			negate();
		return;
	}

	if (iscomplexnumber(p1)) {
		push(p1);
		push(p1);
		conj();
		multiply();
		push_rational(1, 2);
		power();
		return;
	}

	// abs(1/a) evaluates to 1/abs(a)

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(p1);
		reciprocate();
		abs();
		reciprocate();
		return;
	}

	// abs(a*b) evaluates to abs(a)*abs(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			abs();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	if (isnegativeterm(p1) || (car(p1) == symbol(ADD) && isnegativeterm(cadr(p1)))) {
		push(p1);
		negate();
		p1 = pop();
	}

	push_symbol(ABS);
	push(p1);
	list(2);
}
