function
emit_update_superscript(u, small_font)
{
	emit_update(u);

	// move above baseline, emit_svg() may move higher

	if (small_font) {
		u.height = u.height + u.depth + SMALL_X_HEIGHT;
		u.depth = -SMALL_X_HEIGHT;
	} else {
		u.height = u.height + u.depth + X_HEIGHT;
		u.depth = -X_HEIGHT;
	}
}
