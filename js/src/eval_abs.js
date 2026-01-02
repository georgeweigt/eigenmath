function
eval_abs(p1)
{
	push(cadr(p1));
	evalf();
	absfunc();
}

function
absfunc()
{
	var i, h, n, p1, p2;

	p1 = pop();

	if (istensor(p1)) {
		if (p1.dim.length > 1) {
			push_symbol(ABS);
			push(p1);
			list(2);
			return;
		}
		n = p1.dim[0];
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			absfunc();
			dupl();
			multiply();
		}
		add_terms(n);
		sqrtfunc();
		return;
	}

	// abs(-1) -> 1

	if (isnum(p1)) {
		push(p1);
		if (isnegativenumber(p1))
			negate();
		return;
	}

	// abs(3 + 4 i) -> 5

	if (iscomplexnumber(p1)) {
		push(p1);
		push(p1);
		conjfunc();
		multiply();
		sqrtfunc();
		return;
	}

	// abs(3 - pi) -> pi - 3

	push(p1);
	floatfunc();
	p2 = pop();
	if (isnum(p2)) {
		push(p1);
		if (isnegativenumber(p2))
			negate();
		return;
	}

	// abs(exp(i theta)) -> 1

	push(p1);
	push(p1);
	conjfunc();
	multiply();
	p2 = pop();
	if (isnum(p2)) {
		push(p2);
		sqrtfunc();
		return;
	}

	// abs(1/a) evaluates to 1/abs(a)

	if (car(p1) == symbol(POWER) && isnegativeterm(caddr(p1))) {
		push(p1);
		reciprocate();
		absfunc();
		reciprocate();
		return;
	}

	// abs(a*b) evaluates to abs(a)*abs(b)

	if (car(p1) == symbol(MULTIPLY)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			absfunc();
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
