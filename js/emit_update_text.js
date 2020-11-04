function
emit_update_text(u)
{
	var n, w;

	n = u.s.charCodeAt(0);

	if (u.italic_font)
		w = italic_width[n];
	else
		w = roman_width[n];

	if (w == undefined)
		w = 1000;

	if (u.small_font) {
		u.height = SMALL_FONT_HEIGHT;
		u.depth = SMALL_FONT_DEPTH;
		u.width = w * WIDTH_RATIO * SMALL_FONT_SIZE;
	} else {
		u.height = FONT_HEIGHT;
		u.depth = FONT_DEPTH;
		u.width = w * WIDTH_RATIO * FONT_SIZE;
	}
}
