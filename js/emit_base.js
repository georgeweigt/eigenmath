function
emit_base(u, p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(ADD) || car(p) == symbol(MULTIPLY) || car(p) == symbol(POWER))
		emit_subexpr(u, p);
	else
		emit_expr(u, p);
}
