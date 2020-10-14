function
emit_tensor(p)
{
	// if odd rank then emit vector

	if (p.dim.length % 2 == 1)
		emit_vector(p);
	else
		emit_matrix(p, 0, 0);
}
