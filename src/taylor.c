void
eval_taylor(struct atom *p1)
{
	int h, i, n;
	struct atom *F, *X, *A, *C;

	push(cadr(p1));
	eval();
	F = pop();

	push(caddr(p1));
	eval();
	X = pop();

	push(cadddr(p1));
	eval();
	n = pop_integer();

	p1 = cddddr(p1);

	if (iscons(p1)) {
		push(car(p1));
		eval();
	} else
		push_integer(0); // default expansion point

	A = pop();

	h = tos;

	push(F);	// f(a)
	push(X);
	push(A);
	subst();
	eval();

	push_integer(1);
	C = pop();

	for (i = 1; i <= n; i++) {

		push(F);	// f = f'
		push(X);
		derivative();
		F = pop();

		if (iszero(F))
			break;

		push(C);	// c = c * (x - a)
		push(X);
		push(A);
		subtract();
		multiply();
		C = pop();

		push(F);	// f(a)
		push(X);
		push(A);
		subst();
		eval();

		push(C);
		multiply();
		push_integer(i);
		factorial();
		divide();
	}

	add_terms(tos - h);
}
