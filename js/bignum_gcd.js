function
bignum_gcd(u, v)
{
	var r;

	while (v.length > 1 || v[0] > 0) {
		r = bignum_mod(u, v);
		u = v;
		v = r;
	}

	return bignum_copy(u);
}
