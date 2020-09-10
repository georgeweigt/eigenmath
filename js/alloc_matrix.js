function
alloc_matrix(nrow, ncol)
{
	return {dim:[nrow, ncol], elem:new Array(nrow * ncol)};
}
