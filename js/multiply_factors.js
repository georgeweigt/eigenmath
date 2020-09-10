// n is number of factors on stack

function
multiply_factors(n)
{
	var h, coeff, tfact;

	if (n < 2)
		return;

	if (n == 2 && isnum(stack[stack.length - 2]) && isnum(stack[stack.length - 1])) {
		var p2 = pop();
		var p1 = pop();
		multiply_numbers(p1, p2);
		return;
	}

	h = stack.length - n;

	flatten_factors(h);

	tfact = tensor_factor(h);

	coeff = collect_numerical_factors(h, one);

	if (iszero(coeff)) {
		push(coeff);
		return;
	}

	combine_factors(h);

	normalize_power_factors(h);

	coeff = collect_numerical_factors(h, coeff);

//	coeff = reduce_radical_factors(h, coeff);

	if (isdouble(coeff) || !isplusone(coeff))
		push(coeff);

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

	if (istensor(tfact)) {
		push(tfact);
		inner();
	}
}
