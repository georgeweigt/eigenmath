function
emit_update_superscript(u, v) // u is neighbor
{
	var h, k;

	emit_update(v);

	// move up

	if (u.small_font)
		h = SMALL_FONT_HEIGHT;
	else
		h = FONT_HEIGHT;

	k = u.a.length;

	while (k) {
		k = k - 1;
		if (u.a[k].type == SUBSCRIPT)
			continue;
		h = u.a[k].height;
		break;
	}

	v.height = h + v.height + v.depth;
	v.depth = -h - v.depth;
}
