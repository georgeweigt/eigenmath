void
partition_integrand(void)
{
	int h, n;
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	// push const part

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}

	// push var part

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (findf(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	n = tos - h;

	if (n == 0)
		push_integer(1);
	else if (n > 1) {
		list(n);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
	}
}
