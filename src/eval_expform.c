void
eval_expform(struct atom *p1)
{
	push(cadr(p1));
	evalf();
	expform();
}

void
expform(void)
{
	int h, i, n;
	struct atom *p1, *num, *den;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			expform();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		h = tos;
		p1 = cdr(p1);
		while (iscons(p1)) {
			push(car(p1));
			expform();
			p1 = cdr(p1);
		}
		add_terms(tos - h);
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {

		push(p1);
		numden();
		num = pop();
		den = pop();

		p1 = num;
		if (car(p1) == symbol(MULTIPLY)) {
			h = tos;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform();
				p1 = cdr(p1);
			}
			multiply_factors(tos - h);
		} else {
			push(p1);
			expform();
		}
		num = pop();

		p1 = den;
		if (car(p1) == symbol(MULTIPLY)) {
			h = tos;
			p1 = cdr(p1);
			while (iscons(p1)) {
				push(car(p1));
				expform();
				p1 = cdr(p1);
			}
			multiply_factors(tos - h);
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
		scan("1/2 exp(sqrt(-1) z) + 1/2 exp(-sqrt(-1) z)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(SIN)) {
		scan("-1/2 sqrt(-1) exp(sqrt(-1) z) + 1/2 sqrt(-1) exp(-sqrt(-1) z)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(TAN)) {
		scan("sqrt(-1) / (exp(2 sqrt(-1) z) + 1) - sqrt(-1) exp(2 sqrt(-1) z) / (exp(2 sqrt(-1) z) + 1)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(COSH)) {
		scan("1/2 exp(-z) + 1/2 exp(z)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		scan("-1/2 exp(-z) + 1/2 exp(z)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		scan("-1 / (exp(2 z) + 1) + exp(2 z) / (exp(2 z) + 1)");
		push_symbol(Z_LOWER);
		push(cadr(p1));
		expform();
		subst();
		evalf();
		return;
	}

	h = tos;
	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		expform();
		p1 = cdr(p1);
	}
	list(tos - h);
	evalf();
}
