function
decomp()
{
	var p1;

	X = pop();
	F = pop();

	// is the entire expression constant?

	if (!find(F, X)) {
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
