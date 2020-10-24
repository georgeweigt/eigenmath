function
eval_draw(p)
{
	var F, X;
	push_symbol(NIL); // return value
	if (drawmode)
		return;
	F = cadr(p);
	X = caddr(p);
	if (!isusersymbol(X))
		X = symbol(SYMBOL_X);
	save_binding(X);
	drawmode = 1;
	draw(F, X);
	drawmode = 0;
	restore_binding(X);
}
