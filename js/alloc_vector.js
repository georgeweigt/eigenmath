function
alloc_vector(n)
{
	var p = alloc_tensor();
	p.dim[0] = n;
	return p;
}
