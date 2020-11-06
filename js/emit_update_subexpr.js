function
emit_update_subexpr(u)
{
	var d;

	emit_update(u);

	if (u.level == 0)
		d = FONT_DEPTH;
	else
		d = SMALL_FONT_DEPTH;

	if (u.depth < d)
		u.depth = d;

	u.width += 2 * emit_delim_width(u);
}
