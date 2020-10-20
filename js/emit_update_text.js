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

	u.height = emit_round(HEIGHT_RATIO * size);
	u.depth = emit_round(DEPTH_RATIO * size);
	u.width = emit_round(r * WIDTH_RATIO * size);
}
