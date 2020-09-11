function
decomp_sum(F, X)
{
	var h, n, p1;

	// decomp terms involving x

	p1 = cdr(F);
	while (iscons(p1)) {
		if (find(car(p1), X)) {
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
		if (!find(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n) {
		add_terms(n);
		p1 = pop();
		push(p1);
		push(p1);
		negate(); // need both +a, -a for some integrals
	}
}
