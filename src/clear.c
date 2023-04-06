void
eval_clear(struct atom *p1)
{
	(void) p1; // silence compiler

	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	clear_symbols();

	run_init_script();

	restore_symbol();
	restore_symbol();

	if (gc_level == eval_level)
		gc();

	push_symbol(NIL); // result
}
