function
decomp()
{
	var p1, F, X;

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

function
decomp_sum(F, X)
{
	var h, n;
	var p1;

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

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n > 1)
		add_terms(n);
}

function
decomp_product(F, X)
{
	var h, n;
	var p1;

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

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n > 1)
		multiply_factors(n);
}
