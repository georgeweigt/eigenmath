function
emit_update_subexpr(u)
{
	emit_update(u);

	if (u.depth == 0) {
		if (u.small_font)
			u.depth = SMALL_FONT_DEPTH;
		else
			u.depth = FONT_DEPTH;
	}

	u.width += 2 * emit_delim_width(u.small_font);
}
