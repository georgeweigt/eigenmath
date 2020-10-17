function
emit_symbol(u, p)
{
	var k, n, s;

	if (p == symbol(EXP1)) {
		emit_roman(u, "exp(1)", u.small_font);
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman(u, s, u.small_font);
		return;
	}

	n = emit_symbol_scan(s, 0);

	if (n == 1)
		emit_italic(u, s[0], u.small_font);
	else
		emit_special_symbol(u, s.substring(0, n), u.small_font);

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
