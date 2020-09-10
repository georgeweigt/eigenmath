function
partition_integrand()
{
	var h, p1, p2, p3;

	p2 = pop(); // x
	p1 = pop(); // expr

	// push const part

	h = stack.length;
	p3 = cdr(p1);
	while (iscons(p3)) {
		if (!find(car(p3), p2))
			push(car(p3));
		p3 = cdr(p3);
	}

	if (h == stack.length)
		push_integer(1);
	else
		multiply_factors(stack.length - h);

	// push var part

	h = stack.length;
	p3 = cdr(p1);
	while (iscons(p3)) {
		if (find(car(p3), p2))
			push(car(p3));
		p3 = cdr(p3);
	}

	if (h == stack.length)
		push_integer(1);
	else
		multiply_factors(stack.length - h);
}
