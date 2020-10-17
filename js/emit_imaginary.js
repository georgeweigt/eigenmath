function
emit_imaginary(u, p, small_font) // p = (-1)^expr
{
	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_italic(u, "j", small_font);
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_italic(u, "i", small_font);
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		emit_reciprocal(u, p, small_font);
		return;
	}

	emit_base(u, cadr(p), small_font);
	emit_superscript(u, caddr(p));
}
