function
emit_svg_delims(p, x, y)
{
	var h, d;

	if (p.small_font) {
		h = SMALL_FONT_HEIGHT;
		d = SMALL_FONT_DEPTH;
	} else {
		h = FONT_HEIGHT;
		d = FONT_DEPTH;
	}

	if (p.height > h || p.depth > d) {
		emit_svg_ldelim(p, x, y);
		emit_svg_rdelim(p, x, y);
	} else
		emit_svg_parens(p, x, y);
}
