function
emit_symbol(p)
{
	var k, n, s;

	if (p == symbol(EXP1)) {
		emit_roman("exp(1)");
		return;
	}

	s = printname(p);

	if (iskeyword(p) || p == symbol(LAST) || p == symbol(TRACE)) {
		emit_roman(s);
		return;
	}

	n = emit_symbol_scan(s, 0);

	if (n == s.length) {
		emit_symbol_nib(s);
		return;
	}

	// print symbol with subscript

	emit_symbol_nib(s.substring(0, n));

	emit_subscript_begin();

	k = n;

	while (k < s.length) {
		n = emit_symbol_scan(s, k);
		emit_symbol_nib(s.substring(k, k + n));
		k += n;
	}

	emit_subscript_end();
}
