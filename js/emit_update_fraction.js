function
emit_update_fraction(u)
{
	var d, h, w;

	w = roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	if (u.small_font) {
		h = SMALL_X_HEIGHT + SMALL_STROKE_WIDTH / 2;
		d = -SMALL_X_HEIGHT + SMALL_STROKE_WIDTH / 2;
		w = w * SMALL_FONT_SIZE;
	} else {
		h = X_HEIGHT + STROKE_WIDTH / 2;
		d = -X_HEIGHT + STROKE_WIDTH / 2;
		w = w * FONT_SIZE;
	}

	u.height = u.num.height + u.num.depth + h;
	u.depth = u.den.height + u.den.depth + d;
	u.width = Math.max(u.num.width, u.den.width) + w;
}
