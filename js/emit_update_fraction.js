function
emit_update_fraction(u)
{
	var d, h, w;

	w = 0.5 * roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	if (u.small_font) {
		h = 1.5 * SMALL_STROKE_WIDTH + SMALL_MINUS_HEIGHT;
		d = 1.5 * SMALL_STROKE_WIDTH - SMALL_MINUS_HEIGHT;
		w = w * SMALL_FONT_SIZE;
	} else {
		h = 1.5 * STROKE_WIDTH + MINUS_HEIGHT;
		d = 1.5 * STROKE_WIDTH - MINUS_HEIGHT;
		w = w * FONT_SIZE;
	}

	u.height = u.num.height + u.num.depth + h;
	u.depth = u.den.height + u.den.depth + d;
	u.width = Math.max(u.num.width, u.den.width) + w;
}
