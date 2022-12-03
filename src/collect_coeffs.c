// for example, exp(a x + b x) -> exp((a + b) x)

void
collect_coeffs(void)
{
	int h, i, j, k, n;
	struct atom *p1, *p2, *F, *X;

	X = pop();
	F = pop();

	h = tos;

	if (car(F) == symbol(MULTIPLY)) {
		p1 = cdr(F);
		while (iscons(p1)) {
			push(car(p1));
			push(X);
			collect_coeffs();
			p1 = cdr(p1);
		}
		list(tos - h);
		push_symbol(MULTIPLY);
		swap();
		cons(); // makes MULTIPLY head of list
		return;
	}

	if (car(F) == symbol(POWER) && cadr(F) == symbol(EXP1) && caaddr(F) == symbol(ADD))
		p1 = caddr(F); // argument of exponential
	else if (lengthf(F) == 2 && caadr(F) == symbol(ADD))
		p1 = cadr(F); // argument of sin, cos, log, etc
	else {
		push(F);
		return;
	}

	// partition terms

	p1 = cdr(p1);

	while (iscons(p1)) {
		p2 = car(p1);
		if (car(p2) == symbol(MULTIPLY)) {
			push(p2);
			push(X);
			partition_integrand();	// push const part then push var part
		} else if (findf(p2, X)) {
			push_integer(1);	// const part
			push(p2);		// var part
		} else {
			push(p2);		// const part
			push_integer(1);	// var part
		}
		p1 = cdr(p1);
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

	p1 = pop();

	if (car(F) == symbol(POWER)) {
		push_symbol(POWER);
		push_symbol(EXP1);
		push(p1);
		list(3);
	} else {
		push(car(F));
		push(p1);
		list(2);
	}
}
