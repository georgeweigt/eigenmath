function
emit_special_symbol(u, s, small_font, italic_font)
{
	var r, v;

	r = emit_swtab[s];

	s = "&" + s + ";";

	v = {type:TEXT, s:s, small_font:small_font, italic_font:italic_font};

	if (small_font) {
		v.height = SMALL_FONT_HEIGHT;
		v.depth = SMALL_FONT_DEPTH;
		v.width = Math.round(r * 0.7 * SMALL_FONT_SIZE);
	} else {
		v.height = FONT_HEIGHT;
		v.depth = FONT_DEPTH;
		v.width = Math.round(r * 0.7 * FONT_SIZE);
	}

	u.a.push(v);
}
