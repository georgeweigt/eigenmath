function
emit_italic_symbol(u, s, small_font)
{
	var v = {type:TEXT, s:s, small_font:small_font, italic_font:1};

	if (small_font) {
		v.height = SMALL_FONT_HEIGHT;
		v.depth = SMALL_FONT_DEPTH;
		v.width = SMALL_FONT_WIDTH;
	} else {
		v.height = FONT_HEIGHT;
		v.depth = FONT_DEPTH;
		v.width = FONT_WIDTH;
	}

	u.a.push(v);
}
