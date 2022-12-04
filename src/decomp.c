void
decomp(void)
{
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	// is the entire expression constant?

	if (!findf(F, X)) {
		push(F);
		return;
	}

	// sum?

	if (car(F) == symbol(ADD)) {
		decomp_sum(F, X);
		return;
	}

	// product?

	if (car(F) == symbol(MULTIPLY)) {
		decomp_product(F, X);
		return;
	}

	// power?

	if (car(F) == symbol(POWER)) {
		if (cadr(F) != symbol(EXP1)) {
			push(cadr(F));
			push(X);
			decomp();
		}
		push(caddr(F));
		push(X);
		decomp();
		return;
	}

	// naive decomp

	p1 = cdr(F);
	while (iscons(p1)) {
		push(car(p1));
		push(X);
		decomp();
		p1 = cdr(p1);
	}
}

void
decomp_sum(struct atom *F, struct atom *X)
{
	int h, i, j, k, n;
	struct atom *p1, *p2;

	h = tos;

	// partition terms

	p1 = cdr(F);

	while (iscons(p1)) {
		p2 = car(p1);
		if (findf(p2, X)) {
			if (car(p2) == symbol(MULTIPLY)) {
				push(p2);
				push(X);
				partition_integrand();	// push const part then push var part
			} else {
				push_integer(1);	// const part
				push(p2);		// var part
			}
		}
		p1 = cdr(p1);
	}

	// combine const parts of matching var parts

	n = tos - h;

	for (i = 0; i < n - 2; i += 2)
		for (j = i + 2; j < n; j += 2) {
			if (!equal(stack[h + i + 1], stack[h + j + 1]))
				continue;
			push(stack[h + i]); // add const parts
			push(stack[h + j]);
			add();
			stack[h + i] = pop();
			for (k = j; k < n - 2; k++)
				stack[h + k] = stack[h + k + 2];
			j -= 2; // use same j again
			n -= 2;
			tos -= 2; // pop
		}

	// push const parts, decomp var parts

	list(tos - h);
	p1 = pop();

	while (iscons(p1)) {
		if (!isplusone(car(p1)))
			push(car(p1)); // const part
		push(cadr(p1)); // var part
		push(X);
		decomp();
		p1 = cddr(p1);
	}

	// add together all constant terms

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // makes ADD head of list
	}
}

void
decomp_product(struct atom *F, struct atom *X)
{
	int h, n;
	struct atom *p1;

	// decomp factors involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X)) {
			push(car(p1));
			push(X);
			decomp();
		}
		p1 = cdr(p1);
	}

	// combine constant factors

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}
}
