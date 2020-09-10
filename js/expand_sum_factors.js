function
expand_sum_factors(h)
{
	var i, j, n, p1, p2;

	n = stack.length;

	if (n - h < 2)
		return;

	// search for a sum factor

	for (i = 0; i < n; i++) {
		p2 = stack[i];
		if (car(p2) == symbol(ADD))
			break;
	}

	if (i == n)
		return; // no sum factors

	// remove the sum factor

	stack.splice(i, 1);

	n--;

	if (n > 1) {
		sort_factors(n);
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons();
	}

	p1 = pop(); // p1 is the multiplier

	p2 = cdr(p2); // p2 is the sum factor

	while (iscons(p2)) {
		push(p1);
		push(car(p2));
		multiply();
		p2 = cdr(p2);
	}

	add_terms(stack.length - h);
}
