void
eval_clear(struct atom *p1)
{
	(void) p1; // silence compiler

	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	clear_symbols();

	run_init_script();

	restore_symbol(symbol(TTY));
	restore_symbol(symbol(TRACE));

	if (gc_level + 1 == eval_level)
		gc();

	push_symbol(NIL); // result
}
