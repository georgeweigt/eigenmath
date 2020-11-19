function
emit_power(u, p) // p = y^x
{
	if (cadr(p) == symbol(EXP1)) {
		emit_roman_text(u, "exp");
		emit_subexpr(u, caddr(p));
		return;
	}

	if (isimaginaryunit(p)) {
		if (isimaginaryunit(get_binding(symbol(SYMBOL_J)))) {
			emit_italic_text(u, "j");
			return;
		}
		if (isimaginaryunit(get_binding(symbol(SYMBOL_I)))) {
			emit_italic_text(u, "i");
			return;
		}
	}

	if (isnegativenumber(caddr(p))) {
		emit_reciprocal(u, p);
		return;
	}

	emit_base(u, cadr(p));
	emit_exponent(u, caddr(p));
}
