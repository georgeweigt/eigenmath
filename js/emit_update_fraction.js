function
emit_update_fraction(u)
{
	var d, h, m, v, w, y;

	w = 0.5 * roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	if (u.level == 0) {
		y = FONT_HEIGHT;
		w *= FONT_SIZE;
	} else {
		y = SMALL_FONT_HEIGHT;
		w *= SMALL_FONT_SIZE;
	}

	m = 0.5 * y;	// midpoint
	v = 0.25 * y;	// extra vertical space

	h = v + m;
	d = v - m;

	u.height = u.num.height + u.num.depth + h;
	u.depth = u.den.height + u.den.depth + d;
	u.width = Math.max(u.num.width, u.den.width) + w;
}
