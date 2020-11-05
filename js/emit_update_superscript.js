function
emit_update_superscript(u, h) // h is height of neighbor
{
	var t;

	emit_update(u);

	// move up

	t = u.height + u.depth;

	u.height = h + t;
	u.depth = -h - u.depth;
}
