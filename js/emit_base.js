function
emit_base(p)
{
	if (isnegativenumber(p) || isfraction(p) || isdouble(p) || car(p) == symbol(POWER))
		emit_subexpr(p);
	else
		emit_factor(p);
}
