function
eval_draw(p1)
{
	var F, X;

	push_symbol(NIL); // return value

	if (drawmode)
		return;

	drawmode = 1;

	F = cadr(p1);
	X = caddr(p1);

	if (!isusersymbol(X))
		X = symbol(SYMBOL_X);

	save_binding(X);

	draw(F, X);

	restore_binding(X);

	drawmode = 0;
}
