function
eval_defint(p1)
{
	var A, B, F, X;

	expanding++; // in case integral is in denominator

	push(cadr(p1));
	evalf();
	F = pop();

	p1 = cddr(p1);

	do {
		if (length(p1) < 3)
			stop("defint: missing argument");

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		X = pop();

		push(car(p1));
		p1 = cdr(p1);
		evalf();
		A = pop();

		push(car(p1));
		p1 = cdr(p1);
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

	} while (iscons(p1));

	push(F);

	expanding--;
}
