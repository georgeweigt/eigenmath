// for example, exp(a x + b x) -> exp((a + b) x)

function
collect_coeffs()
{
	var h, i, j, k, n;
	var p1, p2, p3;

	p2 = pop(); // x
	p1 = pop(); // expr

	if (!iscons(p1)) {
		push(p1);
		return;
	}

	h = stack.length;

	// depth first

	push(car(p1));
	p1 = cdr(p1);
	while (iscons(p1)) {
		push(car(p1));
		push(p2);
		collect_coeffs();
		p1 = cdr(p1);
	}
	list(stack.length - h);
	p1 = pop();

	if (car(p1) != symbol(ADD)) {
		push(p1);
		return;
	}

	// partition terms

	p1 = cdr(p1);

	while (iscons(p1)) {
		p3 = car(p1);
		if (car(p3) == symbol(MULTIPLY)) {
			push(p3);
			push(p2);
			partition_integrand(); // push const part then push var part
		} else if (findf(p3, p2)) {
			push_integer(1);	// const part
			push(p3);		// var part
		} else {
			push(p3);		// const part
			push_integer(1);	// var part
		}
		p1 = cdr(p1);
	}

	// combine const parts of matching var parts

	n = stack.length - h;

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
			stack.splice(stack.length - 2); // pop
		}

	// combine all the parts without expanding

	n = stack.length - h;

	for (i = 0; i < n; i += 2) {
		push(stack[h + i + 0]); // const part
		push(stack[h + i + 1]); // var part
		multiply_noexpand();
		stack[h + i / 2] = pop();
	}

	stack.splice(stack.length - n / 2); // pop

	add_terms(stack.length - h);
}
