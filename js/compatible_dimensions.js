function
compatible_dimensions(p1, p2)
{
	var i, n;

	if (!istensor(p1) && !istensor(p2))
		return 1; // both are scalars

	if (!istensor(p1) || !istensor(p2))
		return 0; // scalar and tensor

	n = p1.dim.length;

	if (n != p2.dim.length)
		return 0;

	for (i = 0; i < n; i++)
		if (p1.dim[i] != p2.dim[i])
			return 0;

	return 1;
}
