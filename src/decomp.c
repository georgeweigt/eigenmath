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

	// naive decomp if not sum or product

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
	int h, n;
	struct atom *p1;

	// decomp terms involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X)) {
			push(car(p1));
			push(X);
			decomp();
		}
		p1 = cdr(p1);
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

	if (n > 1)
		add_terms(n);
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

	// multiply together all constant factors

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n > 1)
		multiply_factors(n);
}
