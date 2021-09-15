// u ^ v

function
bignum_pow(u, v)
{
	var w;

	if (v.length == 1 && v[0] == 0)
		return bignum_int(1); // v = 0

	if (u.length == 1 && u[0] == 0)
		return bignum_int(0); // u = 0

	v = bignum_copy(v);

	w = bignum_int(1);

	for (;;) {

		if (v[0] % 2)
			w = bignum_mul(w, u);

		bignum_shr(v);

		if (v.length == 1 && v[0] == 0)
			break; // v = 0

		u = bignum_mul(u, u);
	}

	return w;
}

// shift right

function
bignum_shr(u)
{
	var i;
	for (i = 0; i < u.length - 1; i++) {
		u[i] = Math.floor(u[i] / 2);
		if (u[i + 1] % 2)
			u[i] += 0x800000;
	}
	u[i] = Math.floor(u[i] / 2);
	bignum_norm(u);
}
