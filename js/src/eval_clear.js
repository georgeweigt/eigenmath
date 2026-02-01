function
eval_clear()
{
	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	binding = {};
	usrfunc = {};

	binding[symbol(I_LOWER).printname] = imaginaryunit;
	usrfunc[symbol(I_LOWER).printname] = symbol(NIL);

	initscript();

	restore_symbol();
	restore_symbol();

	push_symbol(NIL); // result
}
