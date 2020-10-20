function
emit_symbol(u, p, small_font)
{
	var k, n, s, v;

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
	else if (s[0] >= 'A' && s[0] <= 'Z')
		emit_roman_symbol(u, s.substring(0, n), small_font);
	else
		emit_italic_symbol(u, s.substring(0, n), small_font);

	if (n == s.length)
		return;

	// emit subscript

	v = {type:SUBSCRIPT, a:[], small_font:small_font};

	k = n;

	while (k < s.length) {

		n = emit_symbol_scan(s, k);

		if (n == 1)
			emit_italic_text(v, s[k], 1);
		else if (s[k] >= 'A' && s[k] <= 'Z')
			emit_roman_symbol(v, s.substring(k, k + n), 1);
		else
			emit_italic_symbol(v, s.substring(k, k + n), 1);

		k += n;
	}

	emit_update(v);

	if (small_font) {
		v.height -= SMALL_X_HEIGHT;
		v.depth += SMALL_X_HEIGHT;
	} else {
		v.height -= X_HEIGHT;
		v.depth += X_HEIGHT;
	}

	u.a.push(v);
}
