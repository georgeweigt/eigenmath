function
emit_tensor(u, p)
{
	if (p.dim.length % 2 == 1)
		emit_vector(u, p); // odd rank
	else
		emit_matrix(u, p, 0, 0); // even rank
}
