function
emit_symbol(u, p, small_font)
{
	var k, n, s;

	if (p == symbol(EXP1)) {
		emit_roman_text(u, "exp(1)", small_font);
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman_text(u, s, small_font);
		return;
	}

	n = emit_symbol_scan(s, 0);

	if (n == 1)
		emit_italic_text(u, s[0], small_font);
	else
		emit_special_symbol(u, s.substring(0, n), small_font);

	if (n == s.length)
		return;

	// emit subscript

	k = n;

	while (k < s.length) {
		n = emit_symbol_scan(s, k);
		emit_subscript(u, s.substring(k, k + n));
		k += n;
	}
}
