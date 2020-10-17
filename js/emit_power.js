function
emit_power(u, p, small_font)
{
	// p = (power -1 x)

	if (isminusone(cadr(p))) {
		emit_imaginary(u, p, small_font);
		return;
	}

	// p = (power e x)

	if (cadr(p) == symbol(EXP1)) {
		emit_roman_text(u, "exp", small_font);
		emit_subexpr(u, caddr(p), small_font);
		return;
	}

	// negative exponent

	if (isnegativenumber(caddr(p))) {
		emit_reciprocal(u, p, small_font);
		return;
	}

	// p = (power y x)

	emit_base(u, cadr(p), small_font);
	emit_superscript(u, caddr(p));
}
