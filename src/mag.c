void
eval_mag(struct atom *p1)
{
	push(cadr(p1));
	eval();
	mag();
}

// use numerator and denominator to handle (a+i*b)/(c+i*d)

void
mag(void)
{
	int i, n;
	struct atom *p1;

	p1 = pop();

	if (istensor(p1)) {
		p1 = copy_tensor(p1);
		n = p1->u.tensor->nelem;
		for (i = 0; i < n; i++) {
			push(p1->u.tensor->elem[i]);
			mag();
			p1->u.tensor->elem[i] = pop();
		}
		push(p1);
		return;
	}

	push(p1);
	numerator();
	mag1();

	push(p1);
	denominator();
	mag1();

	divide();
}

void
mag1(void)
{
	int h;
	struct atom *p1, *RE, *IM;

	p1 = pop();

	if (isnum(p1)) {
		push(p1);
		absfunc();
		return;
	}

	if (car(p1) == symbol(POWER) && isminusone(cadr(p1))) {
		// -1 to a power
		push_integer(1);
		return;
	}

	if (car(p1) == symbol(POWER) && cadr(p1) == symbol(EXP1)) {
		// exponential
		push(caddr(p1));
		real();
		expfunc();
		return;
	}

	if (car(p1) == symbol(MULTIPLY)) {
		// product
		p1 = cdr(p1);
		h = tos;
		while (iscons(p1)) {
			push(car(p1));
			mag();
			p1 = cdr(p1);
		}
		multiply_factors(tos - h);
		return;
	}

	if (car(p1) == symbol(ADD)) {
		// sum
		push(p1);
		rect(); // convert polar terms, if any
		p1 = pop();
		push(p1);
		real();
		RE = pop();
		push(p1);
		imag();
		IM = pop();
		push(RE);
		push(RE);
		multiply();
		push(IM);
		push(IM);
		multiply();
		add();
		push_rational(1, 2);
		power();
		return;
	}

	// real

	push(p1);
}
