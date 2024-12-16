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
		push(cadr(p1));
		expform();
		expcos();
		return;
	}

	if (car(p1) == symbol(SIN)) {
		push(cadr(p1));
		expform();
		expsin();
		return;
	}

	if (car(p1) == symbol(TAN)) {
		push(cadr(p1));
		expform();
		exptan();
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push(cadr(p1));
		expform();
		expcosh();
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push(cadr(p1));
		expform();
		expsinh();
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push(cadr(p1));
		expform();
		exptanh();
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
