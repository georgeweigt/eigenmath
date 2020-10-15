function
emit_imaginary(p) // p = (power -1 x)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J))))
			return emit_italic("j");
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I))))
			return emit_italic("i");
	}

	if (isnegativenumber(caddr(p)))
		return emit_fraction(p);

	emit_base(cadr(p));

	emit_superscript_begin();
	emit_expr(caddr(p));
	emit_superscript_end();
}
