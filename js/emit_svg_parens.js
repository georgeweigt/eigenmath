function
emit_svg_parens(p, x, y)
{
	var d, h, l, r, w;

	if (p.small_font) {
		h = HEIGHT_RATIO * SMALL_FONT_SIZE;
		d = DEPTH_RATIO * SMALL_FONT_SIZE;
		w = SMALL_DELIM_WIDTH;
	} else {
		h = HEIGHT_RATIO * FONT_SIZE;
		d = DEPTH_RATIO * FONT_SIZE;
		w = DELIM_WIDTH;
	}

	l = {type:TEXT, s:"(", height:h, depth:d, width:w, small_font:p.small_font, italic_font:0};
	r = {type:TEXT, s:")", height:h, depth:d, width:w, small_font:p.small_font, italic_font:0};

	emit_svg_text(l, x, y);
	emit_svg_text(r, x + p.width - w, y);
}
