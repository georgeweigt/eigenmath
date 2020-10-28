function
emit_symbol_text(u, s, small_font)
{
	if (isdigit(s))
		emit_roman_text(u, s, small_font);
	else if (s.length == 1)
		emit_italic_text(u, s, small_font);
	else
		emit_glyph(u, s, small_font);
}
