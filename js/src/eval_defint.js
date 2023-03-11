function
eval_defint(p1)
{
	var A, B, F, X;

	push(cadr(p1));
	evalf();
	F = pop();

	p1 = cddr(p1);

	while (iscons(p1)) {

		push(car(p1));
		evalf();
		X = pop();

		push(cadr(p1));
		evalf();
		A = pop();

		push(caddr(p1));
		evalf();
		B = pop();

		push(F);
		push(X);
		integral();
		F = pop();

		push(F);
		push(X);
		push(B);
		subst();
		evalf();

		push(F);
		push(X);
		push(A);
		subst();
		evalf();

		subtract();
		F = pop();

		p1 = cdddr(p1);
	}

	push(F);
}
