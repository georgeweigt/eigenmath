function
emit_svg_parens(p, x, y)
{
	var l, r, w;

	if (p.small_font)
		w = SMALL_DELIM_WIDTH;
	else
		w = DELIM_WIDTH;

	l = {type:TEXT, s:"(", height:p.height, depth:p.depth, width:w, small_font:p.small_font, italic_font:0};
	r = {type:TEXT, s:")", height:p.height, depth:p.depth, width:w, small_font:p.small_font, italic_font:0};

	emit_svg_text(l, x, y);
	emit_svg_text(r, x + p.width - w, y);
}
