function
emit_update_superscript(u, v) // u is neighbor
{
	var h, n;

	emit_update(v);

	// move up

	n = u.a.length;

	if (n == 0) {
		if (u.small_font)
			h = SMALL_FONT_HEIGHT;
		else
			h = FONT_HEIGHT;
	} else {
		if (u.a[n - 1].type == SUBSCRIPT && n > 1)
			h = u.a[n - 2].height;
		else
			h = u.a[n - 1].height;
	}

	v.height = h + v.height + v.depth;
	v.depth = -h - v.depth;
}
