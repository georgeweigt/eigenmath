function
arctanh()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			arctanh();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isplusone(p1) || isminusone(p1))
		stopf("arctanh");

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1 < d && d < 1) {
			d = Math.atanh(d);
			push_double(d);
			return;
		}
	}

	// arctanh(z) = 1/2 log(1 + z) - 1/2 log(1 - z)

	if (isdouble(p1) || isdoublez(p1)) {
		push_double(1.0);
		push(p1);
		add();
		logfunc();
		push_double(1.0);
		push(p1);
		subtract();
		logfunc();
		subtract();
		push_double(0.5);
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arctanh(-x) = -arctanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arctanh();
		negate();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCTANH);
	push(p1);
	list(2);
}
