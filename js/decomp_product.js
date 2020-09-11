function
decomp_product(F, X)
{
	var h, n, p1;

	// decomp factors involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (find(car(p1), X)) {
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
		if (!find(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n > 1)
		multiply_factors(n);
}
