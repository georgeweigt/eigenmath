function
emit_update_text(u)
{
	var n, r, size;

	if (u.small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	n = u.s.charCodeAt(0);

	if (n < 128)
		r = emit_wtab[n];
	else
		r = 1.0;

	u.height = HEIGHT_RATIO * size;
	u.depth = DEPTH_RATIO * size;
	u.width = r * WIDTH_RATIO * size;
}
