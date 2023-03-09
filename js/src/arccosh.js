function
arccosh()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			arccosh();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		if (d >= 1) {
			d = Math.acosh(d);
			push_double(d);
			return;
		}
	}

	// arccosh(z) = log(sqrt(z^2 - 1) + z)

	if (isdouble(p1) || isdoublez(p1)) {
		push(p1);
		push(p1);
		multiply();
		push_double(-1.0);
		add();
		sqrtfunc();
		push(p1);
		add();
		logfunc();
		return;
	}

	if (isplusone(p1)) {
		push_integer(0);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(ARCCOSH);
	push(p1);
	list(2);
}
