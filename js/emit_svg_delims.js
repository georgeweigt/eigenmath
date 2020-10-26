function
emit_svg_delims(u, x, y)
{
	var h, d;

	if (u.small_font) {
		h = SMALL_FONT_HEIGHT;
		d = SMALL_FONT_DEPTH;
	} else {
		h = FONT_HEIGHT;
		d = FONT_DEPTH;
	}

	if (u.height > h || u.depth > d) {
		emit_svg_ldelim(u, x, y);
		emit_svg_rdelim(u, x, y);
	} else
		emit_svg_parens(u, x, y);
}
