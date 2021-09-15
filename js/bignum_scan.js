/* exported bignum_scan */

function
bignum_scan(s)
{
	var m, n, t, u;

	u = bignum_int(0);

	if (s.length <= 7) {
		u[0] = Number(s);
		return u;
	}

	m = bignum_int(10000000); // m = 10^7
	t = bignum_int(0);

	n = s.length % 7;

	if (n == 0)
		u[0] = 0;
	else {
		u[0] = Number(s.substring(0, n));
		s = s.substring(n);
	}

	while (s.length) {
		t[0] = Number(s.substring(0, 7));
		s = s.substring(7);
		u = bignum_mul(u, m);
		u = bignum_add(u, t);
	}

	return u;
}
