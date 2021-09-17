function
iszero(p)
{
	var i, n;

	if (isrational(p))
		return bignum_iszero(p.a);

	if (isdouble(p))
		return p.d == 0;

	if (istensor(p)) {
		n = p.elem.length;
		for (i = 0; i < n; i++) {
			if (!iszero(p.elem[i]))
				return 0;
		}
		return 1;
	}

	return 0;
}
