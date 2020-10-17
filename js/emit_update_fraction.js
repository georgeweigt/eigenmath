function
emit_update_fraction(u)
{
	u.height = u.num.height + u.num.depth + FRAC_PAD + X_HEIGHT;
	u.depth = u.den.height + u.den.depth + FRAC_PAD - X_HEIGHT;
	u.width = Math.max(u.num.width, u.den.width);
}
