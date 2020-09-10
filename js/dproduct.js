function
dproduct(p1, p2)
{
	var i, j, n, p3;

	n = length(p1) - 1;

	for (i = 0; i < n; i++) {

		p3 = cdr(p1);

		for (j = 0; j < n; j++) {
			push(car(p3));
			if (i == j) {
				push(p2);
				derivative();
			}
			p3 = cdr(p3);
		}

		multiply_factors(n);
	}

	add_terms(n);
}
