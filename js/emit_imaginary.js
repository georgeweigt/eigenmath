function
emit_imaginary(p) // p = (power -1 expr)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_italic("j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_italic("i");
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		emit_fraction_begin();
		emit_roman("1");
		emit_fraction_separator();
		emit_number(caddr(p)); // sign is not emitted
		emit_fraction_end();
		return;
	}

	emit_base(cadr(p));

	emit_superscript_begin();
	emit_expr(caddr(p));
	emit_superscript_end();
}
