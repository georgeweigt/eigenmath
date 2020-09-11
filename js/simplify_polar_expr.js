function
simplify_polar_expr(EXPO)
{
	var p1;

	if (car(EXPO) == symbol(ADD)) {
		p1 = cdr(EXPO);
		while (iscons(p1)) {
			if (simplify_polar_term(car(p1))) {
				push(EXPO);
				push(car(p1));
				subtract();
				exp();
				multiply();
				return 1;
			}
			p1 = cdr(p1);
		}
		return 0;
	}

	return simplify_polar_term(EXPO);
}
