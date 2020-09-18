function
multiply_factors(n) // n is number of factors on stack
{
	var h, COEFF, TFACT;

	if (n < 2)
		return;

	h = stack.length - n;

	flatten_factors(h);

	TFACT = tensor_factor(h);

	COEFF = collect_numerical_factors(h, one);

	if (iszero(COEFF)) {
		stack.splice(h); // pop all
		push_integer(0);
		if (istensor(TFACT)) {
			push(TFACT);
			inner();
		}
		return;
	}

	if (h == stack.length) {
		push(COEFF);
		if (istensor(TFACT)) {
			push(TFACT);
			inner();
		}
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

	if (istensor(TFACT)) {
		push(TFACT);
		inner();
	}
}
