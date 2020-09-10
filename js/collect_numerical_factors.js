function
collect_numerical_factors(h, coeff)
{
	var i, n, p1;

	n = stack.length;

	for (i = h; i < n; i++) {

		p1 = stack[i];

		if (isnum(p1)) {
			multiply_numbers(coeff, p1);
			coeff = pop();
			stack.splice(i, 1); // remove factor
			i--;
			n--;
		}
	}

	if (iszero(coeff))
		stack.splice(h, stack.length); // pop all

	return coeff
}
