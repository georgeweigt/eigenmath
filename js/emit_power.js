function
emit_power(p)
{
	// p = (power -1 x)

	if (isminusone(cadr(p))) {
		emit_imaginary(p);
		return;
	}

	// p = (power e x)

	if (cadr(p) == symbol(EXP1)) {
		emit_roman("exp");
		emit_subexpr(caddr(p));		// x
		return;
	}

	// negative exponent

	if (isnegativenumber(caddr(p)))
		return emit_fraction(p);

	// p = (power y x)

	emit_base(cadr(p));			// y
	emit_superscript_begin();
	emit_expr(caddr(p));			// x
	emit_superscript_end();
}
