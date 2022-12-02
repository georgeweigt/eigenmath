void
partition_integrand(void)
{
	int h;
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	// push const part

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (!find(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	if (h == tos)
		push_integer(1);
	else
		multiply_factors(tos - h);

	// push var part

	h = tos;
	p1 = cdr(F);
	while (iscons(p1)) {
		if (find(car(p1), X))
			push(car(p1));
		p1 = cdr(p1);
	}

	if (h == tos)
		push_integer(1);
	else
		multiply_factors(tos - h);
}
