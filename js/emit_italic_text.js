function
emit_italic_text(u, s)
{
	var i, n;
	n = s.length;
	for (i = 0; i < n; i++)
		emit_italic_glyph(u, s.charAt(i));
}
