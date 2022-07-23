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
	int h, i, n;
	struct atom *C, *L, *P, *R, *X;

	X = pop();
	P = pop();

	h = tos;

	coeffs(P, X); // put coeffs on stack

	L = symbol(NIL);

	while (tos - h > 1) {

		C = pop(); // leading coeff

		if (iszero(C))
			continue;

		// divide through by C

		for (i = h; i < tos; i++) {
			push(stack[i]);
			push(C);
			divide();
			stack[i] = pop();
		}

		push_integer(1); // leading coeff

		if (factorpoly_root(h) == 0)
			break;

		R = pop();

		push(R);
		push(L);
		cons(); // prepend R to list L
		L = pop();

		factorpoly_divide(h, R);

		pop(); // remove leading coeff
	}

	tos = h; // pop all

	n = length(L);

	if (n == 0)
		stop("roots");

	if (n == 1) {
		push(car(L));
		return;
	}

	for (i = 0; i < n; i++) {
		push(car(L));
		L = cdr(L);
	}

	sort(n);

	R = alloc_vector(n);

	for (i = 0; i < n; i++)
		R->u.tensor->elem[i] = stack[h + i];

	tos = h; // pop all

	push(R);
}
