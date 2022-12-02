// for example, exp(a x + b x) -> exp((a + b) x)

void
collect_coeffs(void)
{
	int h, i, j, k, n;
	struct atom *p1, *F, *X;

	X = pop();
	F = pop();

	if (!iscons(F)) {
		push(F);
		return;
	}

	h = tos;

	// depth first

	push(car(F));
	F = cdr(F);
	while (iscons(F)) {
		push(car(F));
		push(X);
		collect_coeffs();
		F = cdr(F);
	}
	list(tos - h);
	F = pop();

	if (car(F) != symbol(ADD)) {
		push(F);
		return;
	}

	// partition terms

	F = cdr(F);

	while (iscons(F)) {
		p1 = car(F);
		if (car(p1) == symbol(MULTIPLY)) {
			push(p1);
			push(X);
			partition_integrand();	// push const part then push var part
		} else if (find(p1, X)) {
			push_integer(1);	// const part
			push(p1);		// var part
		} else {
			push(p1);		// const part
			push_integer(1);	// var part
		}
		F = cdr(F);
	}

	// combine const parts of matching var parts

	n = tos - h;

	for (i = 0; i < n - 2; i += 2)
		for (j = i + 2; j < n; j += 2) {
			if (!equal(stack[h + i + 1], stack[h + j + 1]))
				continue;
			push(stack[h + i]); // add const parts
			push(stack[h + j]);
			add();
			stack[h + i] = pop();
			for (k = j; k < n - 2; k++)
				stack[h + k] = stack[h + k + 2];
			j -= 2; // use same j again
			n -= 2;
			tos -= 2; // pop
		}

	// combine all the parts without expanding

	n = tos - h;

	for (i = 0; i < n; i += 2) {
		push(stack[h + i + 0]); // const part
		push(stack[h + i + 1]); // var part
		multiply_noexpand();
		stack[h + i / 2] = pop();
	}

	tos -= n / 2; // pop

	n = tos - h;

	if (n > 1) {
		list(n);
		push_symbol(ADD);
		swap();
		cons(); // makes ADD head of list
	}
}
