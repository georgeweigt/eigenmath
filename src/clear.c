void
eval_clear(struct atom *p1)
{
	(void) p1; // silence compiler

	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	clear_symbols();

	initscript();

	restore_symbol(symbol(TTY));
	restore_symbol(symbol(TRACE));

	push_symbol(NIL); // result
}
