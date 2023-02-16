// push coefficients of polynomial P(X) on stack

function
coeffs(P, X)
{
	var C;

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
