function
eval_tanh(p1)
{
	push(cadr(p1));
	evalf();
	tanhfunc();
}

function
tanhfunc()
{
	var d, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			tanhfunc();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (isdouble(p1)) {
		push(p1);
		d = pop_double();
		d = Math.tanh(d);
		push_double(d);
		return;
	}

	if (isdoublez(p1)) {
		push(p1);
		sinhfunc();
		push(p1);
		coshfunc();
		divide();
		return;
	}

	if (iszero(p1)) {
		push_integer(0);
		return;
	}

	// tanh(-x) = -tanh(x)

	if (isnegativeterm(p1)) {
		push(p1);
		negate();
		tanhfunc();
		negate();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		push(cadr(p1));
		return;
	}

	push_symbol(TANH);
	push(p1);
	list(2);
}
