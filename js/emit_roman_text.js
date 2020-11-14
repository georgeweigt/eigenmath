function
emit_roman_text(u, s)
{
	var i, n;
	n = s.length;
	for (i = 0; i < n; i++)
		emit_roman_glyph(u, s.charAt(i));
}
