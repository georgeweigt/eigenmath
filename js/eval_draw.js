function
eval_draw(p1)
{
	var F, T;

	if (drawmode) {
		push_symbol(NIL); // return value
		return;
	}

	drawmode = 1;

	F = cadr(p1);
	T = caddr(p1);

	if (!isusersymbol(T))
		T = symbol(SYMBOL_X);

	save_binding(T);

	setup_trange();
	setup_xrange();
	setup_yrange();

	setup_final(F, T);

	draw_array = [];

	draw_pass1(F, T);
	draw_pass2(F, T);

	emit_graph();

	restore_binding(T);

	push_symbol(NIL);

	drawmode = 0;
}
