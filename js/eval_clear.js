function
eval_clear()
{
	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	binding = {};
	usrfunc = {};

	initscript();

	restore_symbol(symbol(TTY));
	restore_symbol(symbol(TRACE));

	push_symbol(NIL);
}
