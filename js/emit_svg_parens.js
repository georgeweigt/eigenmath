function
emit_svg_parens(p, x, y)
{
	var dx;

	if (p.small_font)
		dx = SMALL_DELIM_WIDTH / 2;
	else
		dx = DELIM_WIDTH / 2;

	emit_svg_text("(", p.small_font, 0, x + dx, y);

	if (p.small_font)
		dx = p.width - SMALL_DELIM_WIDTH / 2;
	else
		dx = p.width - DELIM_WIDTH / 2;

	emit_svg_text(")", p.small_font, 0, x + dx, y);
}
