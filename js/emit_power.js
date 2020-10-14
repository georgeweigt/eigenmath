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

	// p = (power y -1)

	if (isminusone(caddr(p))) {
		emit_fraction_begin();
		emit_roman("1");
		emit_fraction_separator();
		emit_expr(cadr(p));		// y
		emit_fraction_end();
		return;
	}

	// p = (power y -2)

	if (isnegativenumber(caddr(p))) {
		emit_fraction_begin();
		emit_roman("1");
		emit_fraction_separator();
		emit_base(cadr(p));		// y
		emit_superscript_begin();
		emit_number(caddr(p));		// -2 (sign not emitted)
		emit_superscript_end();
		emit_fraction_end();
		return;
	}

	// p = (power y x)

	emit_base(cadr(p));			// y
	emit_superscript_begin();
	emit_expr(caddr(p));			// x
	emit_superscript_end();
}
