function
emit_glyph(u, s)
{
	var descender, italic_font, v, w;

	s = "&" + s + ";";

	italic_font = glyph_info[s].italic_font;
	descender = glyph_info[s].descender;

	v = {type:TEXT, s:s, small_font:u.small_font, italic_font:italic_font};

	w = glyph_info[s].width;

	if (u.small_font) {
		v.height = SMALL_FONT_HEIGHT;
		if (descender)
			v.depth = SMALL_FONT_DEPTH;
		else
			v.depth = 0;
		v.width = w * WIDTH_RATIO * SMALL_FONT_SIZE;
	} else {
		v.height = FONT_HEIGHT;
		if (descender)
			v.depth = FONT_DEPTH;
		else
			v.depth = 0;
		v.width = w * WIDTH_RATIO * FONT_SIZE;
	}

	u.a.push(v);
}
