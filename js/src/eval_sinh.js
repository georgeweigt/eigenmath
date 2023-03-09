function
eval_sinh(p1)
{
	push(cadr(p1));
	evalf();
	sinhfunc();
}

function
sinhfunc()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			sinhfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = Math.sinh(d);
		push_double(d);
		return;
	}

	// sinh(z) = 1/2 exp(z) - 1/2 exp(-z)

	if (isdoublez(p1)) {
		push_rational(1, 2);
		push(p1);
		expfunc();
		push(p1);
		negate();
		expfunc();
		subtract();
		multiply();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// sinh(-x) -> -sinh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		sinhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(SINH);
	push(p1);
	list(2);
}
