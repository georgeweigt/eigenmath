function
arcsinh()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			arcsinh();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = Math.asinh(d);
		push_double(d);
		return;
	}

	// arcsinh(z) = log(sqrt(z^2 + 1) + z)

	if (isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		log();
		return;
	}

	if (iszero(p1)) {
		push(p1);
		return;
	}

	// arcsinh(-x) = -arcsinh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		arcsinh();
		negate();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCSINH);
	push(p1);
	list(2);
}
