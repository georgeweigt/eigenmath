function
emit_expr(u, p)
{
	if (isnegativeterm(p) || (car(p) == symbol(ADD) && isnegativeterm(cadr(p)))) {
		emit_glyph(u, "minus");
		emit_thin_space(u);
	}
	if (car(p) == symbol(ADD))
		emit_expr_nib(u, p);
	else
		emit_term(u, p);
}
