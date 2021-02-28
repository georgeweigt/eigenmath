function
integral_nib(F, X)
{
	var h;

	save_symbol(symbol(METAA));
	save_symbol(symbol(METAB));
	save_symbol(symbol(METAX));

	set_symbol(symbol(METAX), X, symbol(NIL));

	// put constants in F(X) on the stack

	h = stack.length;

	push_integer(1); // 1 is a candidate for a or b

	push(F);
	push(X);
	decomp();

	integral_lookup(F, h);

	restore_symbol(symbol(METAX));
	restore_symbol(symbol(METAB));
	restore_symbol(symbol(METAA));
}
