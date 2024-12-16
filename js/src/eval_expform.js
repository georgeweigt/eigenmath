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
	var h, i, n, p1;

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

	push(p1);
	numden();
	expform_nib(); // numerator
	evalf();
	swap();
	expform_nib(); // denominator
	evalf();
	divide();
}

function
expform_nib()
{
	var h, p1;

	p1 = pop();

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	if (car(p1) == symbol(COS)) {
		push_symbol(EXPCOS);
		push(cadr(p1));
		expform();
		list(2);
		return;
	}

	if (car(p1) == symbol(SIN)) {
		push_symbol(EXPSIN);
		push(cadr(p1));
		expform();
		list(2);
		return;
	}

	if (car(p1) == symbol(TAN)) {
		push_symbol(EXPTAN);
		push(cadr(p1));
		expform();
		list(2);
		return;
	}

	if (car(p1) == symbol(COSH)) {
		push_symbol(EXPCOSH);
		push(cadr(p1));
		expform();
		list(2);
		return;
	}

	if (car(p1) == symbol(SINH)) {
		push_symbol(EXPSINH);
		push(cadr(p1));
		expform();
		list(2);
		return;
	}

	if (car(p1) == symbol(TANH)) {
		push_symbol(EXPTANH);
		push(cadr(p1));
		expform();
		list(2);
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
}
