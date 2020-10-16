function
emit_power(u, p)
{
	// p = (power -1 x)

	if (isminusone(cadr(p))) {
		emit_imaginary(u, p);
		return;
	}

	// p = (power e x)

	if (cadr(p) == symbol(EXP1)) {
		emit_roman(u, "exp");
		emit_subexpr(u, caddr(p));
		return;
	}

	// negative exponent

	if (isnegativenumber(caddr(p)))
		return emit_fraction(u, p);

	// p = (power y x)

	emit_base(u, cadr(p));
	emit_superscript(u, caddr(p));
}
