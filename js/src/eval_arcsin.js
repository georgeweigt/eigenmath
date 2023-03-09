function
eval_arcsin(p1)
{
	push(cadr(p1));
	evalf();
	arcsin();
}

function
arcsin()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			arcsin();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (-1.0 <= d && d <= 1.0) {
			d = Math.asin(d);
			push_double(d);
			return;
		}
	}

	// arcsin(z) = -i log(i z + sqrt(1 - z^2))

	if (isdouble(p1) || isdoublez(p1)) {
		push(imaginaryunit);
		negate();
		push(imaginaryunit);
		push(p1);
		multiply();
		push_double(1.0);
		push(p1);
		push(p1);
		multiply();
		subtract();
		sqrtfunc();
		add();
		logfunc();
		multiply();
		return;
	}

	// arcsin(-x) = -arcsin(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arcsin();
		negate();
		return;
	}

	// arcsin(1 / sqrt(2)) = 1/4 pi

	if (isoneoversqrttwo(p1)) {
		push_rational(1, 4);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(0) = 0

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// arcsin(1/2) = 1/6 pi

	if (isequalq(p1, 1, 2)) {
		push_rational(1, 6);
		push_symbol(PI);
		multiply();
		return;
	}

	// arcsin(1) = 1/2 pi

	if (isplusone(p1)) {
		push_rational(1, 2);
		push_symbol(PI);
		multiply();
		return;
	}

	push_symbol(ARCSIN);
	push(p1);
	list(2);
}
