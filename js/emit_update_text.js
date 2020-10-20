function
emit_update_text(u)
{
	var n, r, size;

	if (u.small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	n = u.s.charCodeAt(0);

	r = emit_wtab[n];

	u.height = Math.round(1.0 * size);
	u.depth = Math.round(0.3 * size);
	u.width = Math.round(r * 0.7 * size);
}
