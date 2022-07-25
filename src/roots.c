void
eval_roots(struct atom *p1)
{
	push(cadr(p1));
	eval();

	p1 = cddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_symbol(X_LOWER);

	roots();
}

void
roots(void)
{
	int h, i, j, n;
	struct atom *A, *C, *LIST, *P, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	LIST = symbol(NIL);

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue; // coeff of monomial is zero

		// divide through by C

		for (i = h; i < tos; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // new leading coeff

		if (findroot(h) == 0)
			break; // no root found

		A = pop(); // root

		push(A);
		push(LIST);
		cons(); // prepend A to LIST
		LIST = pop();

		reduce(h, A); // divide by X - A
	}

	tos = h; // pop all

	n = length(LIST);

	if (n == 0)
		stop("no roots found");

	if (n == 1) {
		push(car(LIST));
		return;
	}

	for (i = 0; i < n; i++) {
		push(car(LIST));
		LIST = cdr(LIST);
	}

	sort(n); // sort roots

	// eliminate repeated roots

	for (i = 0; i < n - 1; i++)
		if (equal(stack[i], stack[i + 1])) {
			for (j = i + i; j < n - 1; j++)
				stack[j] = stack[j + 1];
			i--;
			n--;
		}

	if (n == 1) {
		tos = h + 1;
		return;
	}

	A = alloc_vector(n);

	for (i = 0; i < n; i++)
		A->u.tensor->elem[i] = stack[h + i];

	tos = h; // pop all

	push(A);
}
