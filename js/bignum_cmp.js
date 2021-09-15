function
bignum_cmp(u, v)
{
	var i;

	if (u.length < v.length)
		return -1; // u < v

	if (u.length > v.length)
		return 1; // u > v

	for (i = u.length - 1; i >= 0; i--) {
		if (u[i] < v[i])
			return -1; // u < v
		if (u[i] > v[i])
			return 1; // u > v
	}

	return 0; // u = v
}
