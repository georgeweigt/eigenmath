function
emit_term(u, p)
{
	if (car(p) == symbol(MULTIPLY))
		emit_term_nib(u, p);
	else
		emit_factor(u, p);
}
