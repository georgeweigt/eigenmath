void
eval_arg(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	polar(); // normalize
	argfunc();
}

// may return a denormalized angle

void
argfunc(void)
{
	int i, n;
	struct atom *p1, *p2, *num, *den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			argfunc();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	numden();
	num = pop();
	den = pop();
	push(num);
	arg_nib();
	push(den);
	arg_nib();
	subtract();

	p2 = pop();

	if (hasdouble(p1) && findf(p2, symbol(PI))) {
		push(p2);
		push_symbol(PI);
		push_double(M_PI);
		subst();
		evalf();
	} else
		push(p2);
}

void
arg_nib(void)
{
	int h;
	struct atom *p1, *x, *y;

	p1 = pop();

	if (isrational(p1)) {
		if (isnegativenumber(p1)) {
			push_symbol(PI);
			negate(); // so that arg(-i) == arg(-1) + arg(i) == -pi + 1/2 pi == -1/2 pi
		} else
			push_integer(0);
		return;
	}

	if (isdouble(p1)) {
		if (isnegativenumber(p1))
			push_double(-M_PI);
		else
			push_double(0.0);
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
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			arg_nib();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		push(p1);
		real();
		x = pop();
		push(p1);
		imag();
		y = pop();
		if (iszero(y)) {
			push_integer(0);
			return;
		}
		if (iszero(x)) {
			push_rational(1, 2);
			push_symbol(PI);
			multiply();
			return;
		}
		push(y);
		push(x);
		arctan();
		return;
	}

	push_integer(0); // p1 is real
}
