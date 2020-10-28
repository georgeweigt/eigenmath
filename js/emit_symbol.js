function
emit_symbol(u, p, small_font)
{
	var j, k, s, v;

	if (p == symbol(EXP1)) {
		emit_roman_text(u, "exp(1)", small_font);
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman_text(u, s, small_font);
		return;
	}

	k = emit_symbol_scan(s, 0);

	emit_symbol_text(u, s.substring(0, k), small_font);

	if (k == s.length)
		return;

	// emit subscript

	v = {type:SUBSCRIPT, a:[], small_font:small_font};

	while (k < s.length) {
		j = k;
		k = emit_symbol_scan(s, k);
		emit_symbol_text(v, s.substring(j, k), 1);
	}

	emit_update(v);

	if (small_font) {
		v.height -= SMALL_MINUS_HEIGHT;
		v.depth += SMALL_MINUS_HEIGHT;
	} else {
		v.height -= MINUS_HEIGHT;
		v.depth += MINUS_HEIGHT;
	}

	u.a.push(v);
}
