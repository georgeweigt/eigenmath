function
emit_svg_delims(h, d, w, small_font, x, y)
{
	var dx;

	if (small_font)
		dx = SMALL_DELIM_WIDTH / 2;
	else
		dx = DELIM_WIDTH / 2;

	emit_svg_text("(", small_font, 0, x + dx, y);

	if (small_font)
		dx = w - SMALL_DELIM_WIDTH / 2;
	else
		dx = w - DELIM_WIDTH / 2;

	emit_svg_text(")", small_font, 0, x + dx, y);
}
