function
emit_power(u, p, small_font) // p = y^x
{
	if (cadr(p) == symbol(EXP1)) {
		emit_roman_text(u, "exp", small_font);
		emit_subexpr(u, caddr(p), small_font);
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J))))
			return emit_italic_text(u, "j");
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I))))
			return emit_italic_text(u, "i");
	}

	if (isnegativenumber(caddr(p)))
		emit_reciprocal(u, p, small_font);
	else {
		emit_base(u, cadr(p), small_font);
		emit_exponent(u, caddr(p), small_font);
	}
}
