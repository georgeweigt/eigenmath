function
emit_update_text(u)
{
	var n, r, size;

	n = u.s.charCodeAt(0);

	r = emit_wtab[n];

	if (u.small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	u.height = HEIGHT_RATIO * size;
	u.depth = DEPTH_RATIO * size;
	u.width = r * WIDTH_RATIO * size;
}
