function
emit_update_text(u)
{
	var n, size, w;

	if (u.small_font)
		size = SMALL_FONT_SIZE;
	else
		size = FONT_SIZE;

	n = u.s.charCodeAt(0);

	if (n > 127)
		w = 1000;
	else if (u.italic_font)
		w = italic_width_tab[n];
	else
		w = roman_width_tab[n];

	u.height = HEIGHT_RATIO * size;
	u.depth = DEPTH_RATIO * size;
	u.width = w * WIDTH_RATIO * size;
}
