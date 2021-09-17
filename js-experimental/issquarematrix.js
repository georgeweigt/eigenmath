function
issquarematrix(p)
{
	return istensor(p) && p.dim.length == 2 && p.dim[0] == p.dim[1];
}
