function
eval_clear()
{
	var t = expanding;
	expanding = 1;

	save_symbol(symbol(TRACE));

	binding = {};
	usrfunc = {};

	initscript();

	restore_symbol(symbol(TRACE));

	push_symbol(NIL);

	expanding = t;
}
