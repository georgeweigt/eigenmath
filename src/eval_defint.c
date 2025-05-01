void
eval_defint(struct atom *p1)
{
	struct atom *F, *X, *A, *B;

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

		if (findf(F, symbol(INTEGRAL)))
			stopf("defint: unsolved integral");

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
