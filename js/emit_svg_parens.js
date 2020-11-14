function
emit_svg_parens(u, x, y)
{
	var d, h, l, r, w;

	if (u.level == 0) {
		h = FONT_HEIGHT;
		d = FONT_DEPTH;
	} else {
		h = SMALL_FONT_HEIGHT;
		d = SMALL_FONT_DEPTH;
	}

	w = emit_delim_width(u);

	l = {type:GLYPH, s:"(", height:h, depth:d, width:w, level:u.level, italic_font:0};
	r = {type:GLYPH, s:")", height:h, depth:d, width:w, level:u.level, italic_font:0};

	emit_svg_glyph(l, x, y);
	emit_svg_glyph(r, x + u.width - w, y);
}
