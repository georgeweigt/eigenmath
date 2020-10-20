function
emit_roman_symbol(u, s, small_font)
{
	var v = {type:TEXT, s:s, small_font:small_font, italic_font:0};

	if (small_font) {
		v.height = SMALL_FONT_HEIGHT;
		v.depth = SMALL_FONT_DEPTH;
		v.width = Math.round(1.0 * SMALL_FONT_SIZE);
	} else {
		v.height = FONT_HEIGHT;
		v.depth = FONT_DEPTH;
		v.width = Math.round(1.0 * FONT_SIZE);
	}

	u.a.push(v);
}
