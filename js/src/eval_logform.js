function
eval_logform(p1)
{
	push(cadr(p1));
	evalf();
	logform();
	evalf();
}

function
logform()
{
	var h, i, n, p1, x, y;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			logform();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(ARCCOS)) {
		scan("-sqrt(-1) log(z + sqrt(-1) sqrt(1 - abs(z)^2))", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		logform();
		subst();
		return;
	}

	if (car(p1) == symbol(ARCSIN)) {
		scan("-sqrt(-1) log(sqrt(-1) z + sqrt(1 - abs(z)^2))", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		logform();
		subst();
		return;
	}

	if (car(p1) == symbol(ARCTAN)) {
		push(cadr(p1));
		logform();
		y = pop();
		push(caddr(p1));
		logform();
		x = pop();
		if (isplusone(x)) {
			scan("-1/2 sqrt(-1) log((sqrt(-1) - z) / (sqrt(-1) + z))", 0);
			push_symbol(Z_LOWER);
			push(y);
			subst();
		} else {
			push_symbol(ARCTAN);
			push(y);
			push(x);
			list(3);
		}
		return;
	}

	if (car(p1) == symbol(ARCCOSH)) {
		scan("log(z + sqrt(abs(z)^2 - 1))", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		logform();
		subst();
		return;
	}

	if (car(p1) == symbol(ARCSINH)) {
		scan("log(z + sqrt(abs(z)^2 + 1))", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		logform();
		subst();
		return;
	}

	if (car(p1) == symbol(ARCTANH)) {
		scan("1/2 log((1 + z) / (1 - z))", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		logform();
		subst();
		return;
	}

	h = stack.length;
	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		logform();
		p1 = cdr(p1);
	}
	list(stack.length - h);
}
