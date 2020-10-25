function
emit_update_fraction(u)
{
	var w = roman_width['n'.charCodeAt(0)] * WIDTH_RATIO;

	u.height = u.num.height + u.num.depth;
	u.depth = u.den.height + u.den.depth;

	u.width = Math.max(u.num.width, u.den.width);

	if (u.small_font) {
		u.height += SMALL_X_HEIGHT;
		u.depth -= SMALL_X_HEIGHT;
		u.width += w * SMALL_FONT_SIZE;
	} else {
		u.height += X_HEIGHT;
		u.depth -= X_HEIGHT;
		u.width += w * FONT_SIZE;
	}
}
