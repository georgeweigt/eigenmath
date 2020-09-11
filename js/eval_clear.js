function
eval_clear(p1)
{
	save_binding(symbol(TRACE));

	binding = {};
	arglist = {};

	initscript();

	restore_binding(symbol(TRACE));

	push_symbol(NIL);
}
