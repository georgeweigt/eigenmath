function
bignum_gcd(u, v)
{
	var r;

	if (u.length == 1 && v.length == 1) {
		u = u[0];
		v = v[0];
		while (v) {
			r = u % v;
			u = v;
			v = r;
		}
		return bignum_int(u);
	}

	while (!bignum_iszero(v)) {
		r = bignum_mod(u, v);
		u = v;
		v = r;
	}

	return bignum_copy(u);
}
