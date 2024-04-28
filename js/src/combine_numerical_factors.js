function
combine_numerical_factors(h, COEF)
{
	var i, p1;
	for (i = h; i < stack.length; i++) {
		p1 = stack[i];
		if (isnum(p1)) {
			multiply_numbers(COEF, p1);
			COEF = pop();
			stack.splice(i, 1); // remove factor
			i--; // use same index again
		}
	}
	return COEF;
}
