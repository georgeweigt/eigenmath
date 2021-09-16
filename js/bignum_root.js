// returns null if not perfect root, otherwise returns u^(1/v)

function
bignum_root(u, v)
{
	var i, j, k, m, r, t;

	if (v.length > 1)
		return null; // v must be 24 bits or less

	if (v[0] == 0)
		return null; // divide by zero

	if (u.length == 1 && v.length == 1) {
		r = Math.round(Math.pow(u[0], 1 / v[0]));
		if (Math.pow(r, v[0]) == u[0])
			return bignum_int(r);
		else
			return null;
	}

	// k is bit length of u

	k = 24 * (u.length - 1);

	m = u[u.length - 1];

	while (m) {
		m = Math.floor(m / 2);
		k++;
	}

	if (k == 0)
		return bignum_int(0); // u = 0

	// initial guess of index of ms bit in result

	k = Math.floor((k - 1) / v[0]);

	j = Math.floor(k / 24) + 1; // k is bit index, not number of bits

	r = [];

	for (i = 0; i < j; i++)
		r[i] = 0;

	while (k >= 0) {

		i = Math.floor(k / 24);
		m = Math.pow(2, k % 24);

		r[i] += m; // set bit

		bignum_norm(r);

		t = bignum_pow(r, v);

		switch (bignum_cmp(t, u)) {
		case -1:
			break;
		case 0:
			return r;
		case 1:
			r[i] -= m; // clear bit
			break;
		}

		k--;
	}

	return null;
}
