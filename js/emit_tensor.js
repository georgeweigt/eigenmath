function
emit_tensor(u, p)
{
	// if odd rank then emit vector

	if (p.dim.length % 2 == 1)
		emit_vector(u, p);
	else
		emit_matrix(u, p, 0, 0);
}
