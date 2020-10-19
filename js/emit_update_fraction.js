function
emit_update_fraction(u)
{
	u.width = Math.max(u.num.width, u.den.width);

	if (u.small_font) {
		u.height = u.num.height + u.num.depth + SMALL_FRAC_PAD + SMALL_X_HEIGHT;
		u.depth = u.den.height + u.den.depth + SMALL_FRAC_PAD - SMALL_X_HEIGHT;
		u.width += SMALL_FRAC_PAD;
	} else {
		u.height = u.num.height + u.num.depth + FRAC_PAD + X_HEIGHT;
		u.depth = u.den.height + u.den.depth + FRAC_PAD - X_HEIGHT;
		u.width += FRAC_PAD;
	}
}
