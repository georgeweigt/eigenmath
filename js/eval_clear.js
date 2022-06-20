function
eval_clear()
{
	save_symbol(symbol(TRACE));

	binding = {};
	usrfunc = {};

	initscript();

	restore_symbol(symbol(TRACE));

	push_symbol(NIL);
}
