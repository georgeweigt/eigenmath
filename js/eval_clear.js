function
eval_clear(p1)
{
	save_binding(symbol(TRACE));
	clear_symbols();
//FIXME	run_init_script();
	restore_binding(symbol(TRACE));
	push_symbol(NIL);
}
