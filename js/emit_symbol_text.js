function
emit_symbol_text(u, s)
{
	if (isdigit(s))
		emit_roman_text(u, s);
	else if (s.length == 1)
		emit_italic_text(u, s);
	else
		emit_glyph(u, s);
}
