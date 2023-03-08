void
eval_arccos(struct atom *p1)
{
	push(cadr(p1));
	eval();
	arccos();
}

void
arccos(void)
{
	int i, n;
	double d;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			arccos();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1.0 <= d && d <= 1.0) {
			d = acos(d);
			push_double(d);
			return;
		}
	}

	// arccos(z) = -i log(z + i sqrt(1 - z^2))

	if (isdouble(p1) || isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		sqrtfunc();
		push(imaginaryunit);
		multiply();
		push(p1);
		add();
		logfunc();
		push(imaginaryunit);
		multiply();
		negate();
		return;
	}

	// arccos(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1 / sqrt(2)) = 3/4 pi

	if (isminusoneoversqrttwo(p1)) {
		push_rational(3, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(0) = 1/2 pi

	if (iszero(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1/2) = 1/3 pi

	if (isequalq(p1, 1 ,2)) {
		push_rational(1, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(1) = 0

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	// arccos(-1/2) = 2/3 pi

	if (isequalq(p1, -1, 2)) {
		push_rational(2, 3);
		push_symbol(PI);
		multiply();
		return;
	}

	// arccos(-1) = pi

	if (isminusone(p1)) {
		push_symbol(PI);
		return;
	}

	push_symbol(ARCCOS);
	push(p1);
	list(2);
}
