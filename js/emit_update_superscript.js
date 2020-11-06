function
emit_update_superscript(u, v)
{
	var h, k;

	emit_update(v);

	// h is height of neighbor

	if (u.small_font)
		h = SMALL_FONT_HEIGHT;
	else
		h = FONT_HEIGHT;

	k = u.a.length;

	while (k) {
		k = k - 1;
		if (u.a[k].type == SUBSCRIPT)
			continue;
		h = Math.max(h, u.a[k].height);
		break;
	}

	// adjust

	h = h - Math.floor(SMALL_FONT_HEIGHT / 2);

	// move up

	v.height = h + v.height + v.depth;
	v.depth = -h - v.depth;
}
