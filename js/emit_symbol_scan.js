function
emit_symbol_scan(s, k)
{
	var i, n, t;
	n = glyph_names.length;
	for (i = 0; i < n; i++) {
		t = glyph_names[i];
		if (s.startsWith(t, k))
			return t.length;
	}
	return 1;
}
