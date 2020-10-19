function
emit_update_superscript(u, h) // h is height of neighbor
{
	var d;

	emit_update(u);

	// move above baseline

	if (u.small_font) {
		u.height = u.height + u.depth + SMALL_X_HEIGHT;
		u.depth = -u.depth - SMALL_X_HEIGHT;
	} else {
		u.height = u.height + u.depth + X_HEIGHT;
		u.depth = -u.depth - X_HEIGHT;
	}

	d = h - u.height;

	if (d > 0) {
		u.height += d;
		u.depth -= d;
	}
}
