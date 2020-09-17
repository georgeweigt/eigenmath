function
eval_clear(p1)
{
	p1; // suppress eslint warning

	save_binding(symbol(TRACE));

	binding = {};
	arglist = {};

	initscript();

	restore_binding(symbol(TRACE));

	push_symbol(NIL);
}
