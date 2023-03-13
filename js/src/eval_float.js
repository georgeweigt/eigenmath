function
eval_float(p1)
{
	push(cadr(p1));
	evalf();
	floatfunc();
}

function
floatfunc()
{
	floatfunc_subst();
	evalf();
	floatfunc_subst(); // in case pi popped up
	evalf();
}

function
floatfunc_subst()
{
	var a, b, h, i, n, p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			floatfunc_subst();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (p1 == symbol(PI)) {
		push_double(Math.PI);
		return;
	}

	if (p1 == symbol(EXP1)) {
		push_double(Math.E);
		return;
	}

	if (isrational(p1)) {
		a = bignum_float(p1.a);
		b = bignum_float(p1.b);
		if (isnegativenumber(p1))
			a = -a;
		push_double(a / b);
		return;
	}

	// don't float exponential

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		push_symbol(POWER);
		push_symbol(EXP1);
		push(caddr(p1));
		floatfunc_subst();
		list(3);
		return;
	}

	// don't float imaginary unit, but multiply it by 1.0

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		push_symbol(MULTIPLY);
		push_double(1.0);
		push_symbol(POWER);
		push(cadr(p1));
		push(caddr(p1));
		floatfunc_subst();
		list(3);
		list(3);
		return;
	}

	if (iscons(p1)) {
		h = stack.length;
		push(car(p1));
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			floatfunc_subst();
			p1 = cdr(p1);
		}
		list(stack.length - h);
		return;
	}

	push(p1);
}
