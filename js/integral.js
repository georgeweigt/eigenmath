function
integral()
{
	var h, p1, F, X;

	X = pop();
	F = pop();

	if (car(F) == symbol(ADD)) {
		h = stack.length;
		p1 = cdr(F);
		while (iscons(p1)) {
			push(car(p1));
			push(X);
			integral();
			p1 = cdr(p1);
		}
		add_terms(stack.length - h);
		return;
	}

	if (car(F) == symbol(MULTIPLY)) {
		push(F);
		push(X);
		partition_integrand();	// push const part then push var part
		F = pop();		// pop var part
		integral_nib(F, X);
		multiply();		// multiply by const part
		return;
	}

	integral_nib(F, X);
}
