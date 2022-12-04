function
integral()
{
	var h, p1, F, X;

	X = pop();
	F = pop();

	if (!isusersymbol(X))
		stopf("integral: symbol expected");

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
		partition_integrand();
		F = pop();		// pop var part
		integral_nib(F, X);
		multiply();		// multiply by const part
		return;
	}

	integral_nib(F, X);
}

function
integral_nib(F, X)
{
	var h;

	save_symbol(symbol(SA));
	save_symbol(symbol(SB));
	save_symbol(symbol(SX));

	set_symbol(symbol(SX), X, symbol(NIL));

	// put constants in F(X) on the stack

	h = stack.length;

	push_integer(1); // 1 is a candidate for a or b

	push(F);
	push(X);
	decomp();

	integral_lookup(F, h);

	restore_symbol(symbol(SX));
	restore_symbol(symbol(SB));
	restore_symbol(symbol(SA));
}
