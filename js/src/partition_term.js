function
partition_term()
{
	var h, n;
	var p1, F, X;

	X = pop();
	F = pop();

	// push const factors

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}

	// push var factors

	h = stack.length;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = stack.length - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}
}
