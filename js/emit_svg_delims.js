function
emit_svg_delims(p, x, y)
{
	if (p.small_font) {
		if (p.height > SMALL_FONT_HEIGHT || p.depth > SMALL_FONT_DEPTH) {
			emit_svg_ldelim(p, x, y);
			emit_svg_rdelim(p, x + p.width - SMALL_DELIM_WIDTH, y);
		} else
			emit_svg_parens(p, x, y);
	} else {
		if (p.height > FONT_HEIGHT || p.depth > FONT_DEPTH) {
			emit_svg_ldelim(p, x, y);
			emit_svg_rdelim(p, x + p.width - DELIM_WIDTH, y);
		} else
			emit_svg_parens(p, x, y);
	}
}
