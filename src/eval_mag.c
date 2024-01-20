void
eval_mag(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	mag();
}

void
mag(void)
{
	int i, n;
	struct atom *p1, *num, *den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			mag();
			p1->u.tensor->elem[i] = pop();
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
	mag_nib();
	push(den);
	mag_nib();
	divide();

	if (isdoublesomewhere(p1))
		floatfunc();
}

void
mag_nib(void)
{
	int h;
	struct atom *p1, *x, *y;

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
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			mag_nib();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
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
