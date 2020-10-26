function
emit_update_fraction(u)
{
	var d, h, w;

	w = 1/2 * roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	if (u.small_font) {
		h = SMALL_MINUS_HEIGHT + 2 * SMALL_STROKE_WIDTH; // more space above fraction line
		d = -SMALL_MINUS_HEIGHT + SMALL_STROKE_WIDTH;
		w = w * SMALL_FONT_SIZE;
	} else {
		h = MINUS_HEIGHT + 2 * STROKE_WIDTH; // more space above fraction line
		d = -MINUS_HEIGHT + STROKE_WIDTH;
		w = w * FONT_SIZE;
	}

	u.height = u.num.height + u.num.depth + h;
	u.depth = u.den.height + u.den.depth + d;
	u.width = Math.max(u.num.width, u.den.width) + w;
}
