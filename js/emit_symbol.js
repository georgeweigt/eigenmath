function
emit_symbol(u, p)
{
	var j, k, s, v;

	if (p == symbol(EXP1)) {
		emit_roman_text(u, "exp(1)");
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman_text(u, s);
		return;
	}

	k = emit_symbol_scan(s, 0);

	emit_symbol_text(u, s.substring(0, k));

	if (k == s.length)
		return;

	// emit subscript

	v = {type:SUBSCRIPT, a:[], level:u.level + 1};

	while (k < s.length) {
		j = k;
		k = emit_symbol_scan(s, k);
		emit_symbol_text(v, s.substring(j, k));
	}

	emit_update(v);

	v.height -= FONT_DEPTH;
	v.depth += FONT_DEPTH;

	u.a.push(v);
}
