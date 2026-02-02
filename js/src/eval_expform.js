function
eval_expform(p1)
{
	push(cadr(p1));
	evalf();
	expform();
}

function
expform()
{
	var h, i, n, p1, num, den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1.elem.length;
		for (i = 0; i < n; i++) {
			push(p1.elem[i]);
			expform();
			p1.elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			expform();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {

		push(p1);
		numden();
		num = pop();
		den = pop();

		p1 = num;
		if (car(p1) == symbol(MULTIPLY)) {
			h = stack.length;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform();
				p1 = cdr(p1);
			}
			multiply_factors(stack.length - h);
		} else {
			push(p1);
			expform();
		}
		num = pop();

		p1 = den;
		if (car(p1) == symbol(MULTIPLY)) {
			h = stack.length;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform();
				p1 = cdr(p1);
			}
			multiply_factors(stack.length - h);
		} else {
			push(p1);
			expform();
		}
		den = pop();

		push(num);
		push(den);
		divide();
		return;
	}

	if (car(p1) == symbol(POWER)) {
		push(cadr(p1));
		expform();
		push(caddr(p1));
		expform();
		power();
		return;
	}

	if (car(p1) == symbol(COS)) {
		scan("1/2 exp(sqrt(-1) z) + 1/2 exp(-sqrt(-1) z)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(SIN)) {
		scan("-1/2 sqrt(-1) exp(sqrt(-1) z) + 1/2 sqrt(-1) exp(-sqrt(-1) z)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(TAN)) {
		scan("sqrt(-1) / (exp(2 sqrt(-1) z) + 1) - sqrt(-1) exp(2 sqrt(-1) z) / (exp(2 sqrt(-1) z) + 1)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(COSH)) {
		scan("1/2 exp(-z) + 1/2 exp(z)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		scan("-1/2 exp(-z) + 1/2 exp(z)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		scan("-1 / (exp(2 z) + 1) + exp(2 z) / (exp(2 z) + 1)", 0);
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	h = stack.length;
	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		expform();
		p1 = cdr(p1);
	}
	list(stack.length - h);
	evalf();
}
