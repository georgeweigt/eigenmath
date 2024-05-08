function
eval_mag(p1)
{
	push(cadr(p1));
	evalf();
	magfunc();
}

function
magfunc()
{
	var i, n, p1, num, den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			magfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	// use numden to handle (a + i b) / (c + i d)

	push(p1);
	numden();
	num = pop();
	den = pop();
	push(num);
	magfunc_nib();
	push(den);
	magfunc_nib();
	divide();

	if (allnum(p1) && hasdouble(p1))
		floatfunc();
}

function
magfunc_nib()
{
	var h, p1, x, y;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		absfunc();
		return;
	}

	// -1 to a power

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_integer(1);
		return;
	}

	// exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push(caddr(p1));
		real();
		expfunc();
		return;
	}

	// product

	if (car(p1) == symbol(MULTIPLY)) {
		p1 = cdr(p1);
		h = stack.length;
		while (iscons(p1)) {
			push(car(p1));
			magfunc_nib();
			p1 = cdr(p1);
		}
		multiply_factors(stack.length - h);
		return;
	}

	// sum

	if (car(p1) == symbol(ADD)) {
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		x = pop();
		push(p1);
		imag();
		y = pop();
		if (iszero(y)) {
			push(x);
			return;
		}
		if (iszero(x)) {
			push(y);
			return;
		}
		push(x);
		push(x);
		multiply();
		push(y);
		push(y);
		multiply();
		add();
		sqrtfunc();
		return;
	}

	// real

	push(p1);
}
