function
emit_update_subexpr(u, small_font)
{
	emit_update(u);

	if (u.depth == 0) {
		if (small_font)
			u.depth = SMALL_FONT_DEPTH;
		else
			u.depth = FONT_DEPTH;
	}

	u.width += 2 * emit_delim_width(small_font);
}
