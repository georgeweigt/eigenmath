function
emit_symbol_scan(s, k)
{
	var i, n, t;
	n = emit_stab.length;
	for (i = 0; i < n; i++) {
		t = emit_stab[i];
		if (s.startsWith(t, k))
			return t.length;
	}
	return 1;
}
