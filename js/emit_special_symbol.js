function
emit_special_symbol(u, s, small_font, italic_font)
{
	var r, size, v;

	r = emit_gtab[s];

	s = "&" + s + ";";

	v = {type:TEXT, s:s, small_font:small_font, italic_font:italic_font};

	if (small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	v.height = HEIGHT_RATIO * size;
	v.depth = DEPTH_RATIO * size;
	v.width = r * WIDTH_RATIO * size;

	u.a.push(v);
}
