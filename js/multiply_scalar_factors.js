function
multiply_scalar_factors(h)
{
	var n, COEFF;

	if (stack.length - h < 2)
		return;

	COEFF = collect_numerical_factors(h, one);

	if (iszero(COEFF) || h == stack.length) {
		stack.splice(h); // pop all
		push(COEFF);
		return;
	}

	combine_factors(h);

	normalize_power_factors(h);

	COEFF = collect_numerical_factors(h, COEFF);

	COEFF = reduce_radical_factors(h, COEFF);

	if (!isplusone(COEFF) || isdouble(COEFF))
		push(COEFF);

	if (expanding)
		expand_sum_factors(h); // success leaves one expr on stack

	n = stack.length - h;

	switch (n) {
	case 0:
		push_integer(1);
		break;
	case 1:
		break;
	default:
		sort_factors(h); // previously sorted provisionally
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
		break;
	}
}
