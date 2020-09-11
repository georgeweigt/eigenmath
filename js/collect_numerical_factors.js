function
collect_numerical_factors(h, COEFF)
{
	var i, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isnum(p1)) {
			multiply_numbers(COEFF, p1);
			COEFF = pop();
			stack.splice(i, 1); // remove factor
			i--;
			n--;
		}
	}

	return COEFF;
}
