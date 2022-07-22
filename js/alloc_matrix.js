function
alloc_matrix(nrow, ncol)
{
	var p = alloc_tensor();
	p.dim[0] = nrow;
	p.dim[1] = ncol;
	return p;
}
