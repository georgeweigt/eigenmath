function
eval_clear()
{
	save_binding(symbol(TRACE));

	binding = {};
	arglist = {};

	initscript();

	restore_binding(symbol(TRACE));

	push_symbol(NIL);
}
