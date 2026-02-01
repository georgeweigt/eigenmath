void
eval_clear(struct atom *p1)
{
	int i;

	(void) p1; // silence compiler

	save_symbol(symbol(TRACE));
	save_symbol(symbol(TTY));

	for (i = 0; i < 27 * BUCKETSIZE; i++) {
		binding[i] = NULL;
		usrfunc[i] = NULL;
	}

	binding[symbol(I_LOWER)->u.usym.index] = imaginaryunit;
	usrfunc[symbol(I_LOWER)->u.usym.index] = symbol(NIL);

	run_init_script();

	restore_symbol();
	restore_symbol();

	if (gc_level == eval_level)
		gc();

	push_symbol(NIL); // result
}
