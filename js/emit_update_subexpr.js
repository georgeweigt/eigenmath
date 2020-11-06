function
emit_update_subexpr(u)
{
	var d, h;

	emit_update(u);

	if (u.level == 0) {
		h = FONT_HEIGHT
		d = FONT_DEPTH;
	} else {
		h = SMALL_FONT_HEIGHT;
		d = SMALL_FONT_DEPTH;
	}

	u.height = Math.max(h, u.height);
	u.depth = Math.max(d, u.depth);

	u.width += 2 * emit_delim_width(u);
}
