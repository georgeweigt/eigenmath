// push coefficients of polynomial P(X) on stack

void
coeffs(struct atom *P, struct atom *X)
{
	struct atom *C;

	for (;;) {

		push(P);
		push(X);
		push_integer(0);
		subst();
		evalf();
		C = pop();

		push(C);

		push(P);
		push(C);
		subtract();
		P = pop();

		if (iszero(P))
			break;

		push(P);
		push(X);
		divide();
		P = pop();
	}
}
