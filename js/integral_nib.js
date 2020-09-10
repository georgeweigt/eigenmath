function
integral_nib(F, X)
{
	var h;

	save_binding(symbol(METAA));
	save_binding(symbol(METAB));
	save_binding(symbol(METAX));

	set_binding(symbol(METAX), X);

	// put constants in F(X) on the stack

	h = stack.length;

	push_integer(1); // 1 is a candidate for a or b

	push(F);
	push(X);
	collect_coeffs();
	push(X);
	decomp();

	integral_lookup(F, h);

	restore_binding(symbol(METAX));
	restore_binding(symbol(METAB));
	restore_binding(symbol(METAA));
}
