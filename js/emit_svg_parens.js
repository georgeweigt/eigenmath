function
emit_svg_parens(p, x, y)
{
	var d, h, l, r, w;

	if (p.small_font) {
		h = SMALL_FONT_HEIGHT;
		d = SMALL_FONT_DEPTH;
		w = SMALL_DELIM_WIDTH;
	} else {
		h = FONT_HEIGHT;
		d = FONT_DEPTH;
		w = DELIM_WIDTH;
	}

	l = {type:TEXT, s:"(", height:h, depth:d, width:w, small_font:p.small_font, italic_font:0};
	r = {type:TEXT, s:")", height:h, depth:d, width:w, small_font:p.small_font, italic_font:0};

	emit_svg_text(l, x, y);
	emit_svg_text(r, x + p.width - w, y);
}
