function
emit_update_superscript(u, v)
{
	var h, k;

	emit_update(v);

	// h is height of neighbor

	k = u.a.length - 1;
	h = u.a[k].height;

	// adjust

	h -= Math.floor((v.height + v.depth) / 2);

	if (u.level == 0)
		h = Math.max(h, SUPERSCRIPT_HEIGHT);
	else
		h = Math.max(h, SMALL_SUPERSCRIPT_HEIGHT);

	v.dx = 0;
	v.dy = -(h + v.depth);

	v.height = h + v.height + v.depth;
	v.depth = 0;

	if (u.a[k].type == SUBSCRIPT) {
		v.dx = -u.a[k].width;
		v.width = Math.max(0, v.width - u.a[k].width);
	}
}
