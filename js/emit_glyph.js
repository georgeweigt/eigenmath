function
emit_glyph(u, s, small_font)
{
	var italic_font, size, v, width;

	s = "&" + s + ";";

	width = glyph_info[s].width;

	italic_font = glyph_info[s].italic_font;

	v = {type:TEXT, s:s, small_font:small_font, italic_font:italic_font};

	if (small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	v.height = HEIGHT_RATIO * size;
	v.depth = DEPTH_RATIO * size;
	v.width = width * WIDTH_RATIO * size;

	u.a.push(v);
}
