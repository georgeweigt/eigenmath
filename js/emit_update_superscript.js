function
emit_update_superscript(u, h) // h is height of neighbor
{
	emit_update(u);

	// move up

	u.height = h + u.height + u.depth;
	u.depth = -h - u.depth;
}
