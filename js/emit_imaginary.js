function
emit_imaginary(u, p) // p = (power -1 x)
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J))))
			return emit_italic(u, "j");
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I))))
			return emit_italic(u, "i");
	}

	if (isnegativenumber(caddr(p)))
		return emit_fraction(u, p);

	emit_base(u, cadr(p));
	emit_superscript(u, caddr(p));
}
