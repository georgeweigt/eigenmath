function
emit_update_fraction(u)
{
	var d, h, w;

	w = 0.5 * roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	if (u.small_font) {
		h = SMALL_FRAC_VSPACE + SMALL_MINUS_HEIGHT;
		d = SMALL_FRAC_VSPACE - SMALL_MINUS_HEIGHT;
		w = w * SMALL_FONT_SIZE;
	} else {
		h = FRAC_VSPACE + MINUS_HEIGHT;
		d = FRAC_VSPACE - MINUS_HEIGHT;
		w = w * FONT_SIZE;
	}

	u.height = u.num.height + u.num.depth + h;
	u.depth = u.den.height + u.den.depth + d;
	u.width = Math.max(u.num.width, u.den.width) + w;
}
