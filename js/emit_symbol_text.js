function
emit_symbol_text(u, s, small_font)
{
	if (s.length == 1) {
		if (isdigit(s))
			emit_roman_text(u, s, small_font);
		else
			emit_italic_text(u, s, small_font);
	} else
		emit_glyph(u, s, small_font);
}
