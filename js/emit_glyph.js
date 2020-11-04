function
emit_glyph(u, s, small_font)
{
	var italic_font, v, w;

	s = "&" + s + ";";

	italic_font = glyph_info[s].italic_font;

	v = {type:TEXT, s:s, small_font:small_font, italic_font:italic_font};

	w = glyph_info[s].width;

	if (small_font) {
		v.height = SMALL_FONT_HEIGHT;
		v.depth = SMALL_FONT_DEPTH;
		v.width = w * WIDTH_RATIO * SMALL_FONT_SIZE;
	} else {
		v.height = FONT_HEIGHT;
		v.depth = FONT_DEPTH;
		v.width = w * WIDTH_RATIO * FONT_SIZE;
	}

	u.a.push(v);
}
