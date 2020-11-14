function
emit_update_superscript(u, v)
{
	var h;

	emit_update(v);

	// h is height of neighbor

	h = u.a[u.a.length - 1].height;

	// adjust

	h -= v.height + v.depth;

	if (u.level == 0)
		h = Math.max(h, SUPERSCRIPT_HEIGHT);
	else
		h = Math.max(h, SMALL_SUPERSCRIPT_HEIGHT);

	// move up

	v.height = h + v.height + v.depth;
	v.depth = -h - v.depth;
}
