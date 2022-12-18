function
eval_draw(p1)
{
	var F, T;

	if (drawing) {
		push_symbol(NIL); // return value
		return;
	}

	drawing = 1;

	F = cadr(p1);
	T = caddr(p1);

	if (!isusersymbol(T))
		T = symbol(X_LOWER);

	save_symbol(T);

	setup_trange();
	setup_xrange();
	setup_yrange();

	setup_final(F, T);

	draw_array = [];

	draw_pass1(F, T);
	draw_pass2(F, T);

	emit_graph();

	restore_symbol(T);

	push_symbol(NIL); // return value

	drawing = 0;
}
