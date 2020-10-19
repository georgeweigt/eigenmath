function
emit_expr(u, p, small_font)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p))))
		emit_symbol_text(u, "&minus;", small_font);
	if (car(p) == symbol(ADD))
		emit_expr_nib(u, p, small_font);
	else
		emit_term(u, p, small_font);
}
