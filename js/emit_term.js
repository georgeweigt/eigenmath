function
emit_term(u, p, small_font)
{
	if (car(p) == symbol(MULTIPLY))
		emit_term_nib(u, p, small_font);
	else
		emit_factor(u, p, small_font);
}
