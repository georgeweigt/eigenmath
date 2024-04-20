function
multiply_scalar_factors(h)
{
	var n, COEFF;

	COEFF = combine_numerical_factors(h, one);

	if (iszero(COEFF) || h == stack.length) {
		stack.length = h; // pop all
		push(COEFF);
		return;
	}

	combine_factors(h);
	normalize_power_factors(h);

	// do again in case exp(1/2 i pi) changed to i

	combine_factors(h);
	normalize_power_factors(h);

	COEFF = combine_numerical_factors(h, COEFF);

	if (iszero(COEFF) || h == stack.length) {
		stack.length = h; // pop all
		push(COEFF);
		return;
	}

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
