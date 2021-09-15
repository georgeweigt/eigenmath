/* exported bignum_add */
/* exported bignum_sub */
/* exported bignum_mul */
/* exported bignum_div */
/* exported bignum_mod */
/* exported bignum_pow */
/* exported bignum_shr */
/* exported bignum_gcd */
/* exported bignum_cmp */
/* exported bignum_eq */
/* exported bignum_root */
/* exported bignum_int */
/* exported bignum_copy */
/* exported bignum_norm */

const BIGM = 0x1000000;

// u ^ v

function
bignum_pow(u, v)
{
	var w;

	v = bignum_copy(v);

	w = bignum_int(1);

	for (;;) {

		if (v[0] & 1)
			w = bignum_mul(w, u);

		bignum_shr(v);

		if (v.length == 1 && v[0] == 0)
			break; // v = 0

		u = bignum_mul(u, u);
	}

	return w;
}

// u = u >> 1

function
bignum_shr(u)
{
	var i;
	for (i = 0; i < u.length - 1; i++) {
		u[i] >>= 1;
		if (u[i + 1] & 1)
			u[i] |= 0x800000;
	}
	u[i] >>= 1;
	bignum_norm(u);
}

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

// compare u and v

function
bignum_cmp(u, v)
{
	var i;

	if (u.length < v.length)
		return -1;

	if (u.length > v.length)
		return 1;

	for (i = u.length - 1; i >= 0; i--) {
		if (u[i] < v[i])
			return -1;
		if (u[i] > v[i])
			return 1;
	}

	return 0; // u = v
}

function
bignum_eq(u, v)
{
	var i;
	if (u.length != v.length)
		return 0;
	for (i = 0; i < u.length; i++)
		if (u[i] != v[i])
			return 0;
	return 1;
}

// returns null if not perfect root, otherwise returns u^(1/n)

function
bignum_root(u, n)
{
	var i, j, k, m, r, t;

	if (n.length > 1)
		return null; // n must be 24 bits or less

	if (n[0] == 0)
		return null; // divide by zero

	// k is bit length of u

	k = 24 * (u.length - 1);

	m = u[u.length - 1];

	while (m) {
		m >>= 1;
		k++;
	}

	if (k == 0)
		return bignum_int(0); // u = 0

	// initial guess of index of ms bit in result

	k = (k - 1) / n[0];

	j = Math.floor(k / 24) + 1; // k is bit index, not number of bits

	r = bignum_int(0);

	for (i = 0; i < j; i++)
		r[i] = 0;

	while (k >= 0) {

		i = Math.floor(k / 24);
		m = 2 << (k % 24);

		r[i] |= m; // set bit

		bignum_norm(r);

		t = bignum_pow(r, n);

		switch (bignum_cmp(t, u)) {
		case -1:
			break;
		case 0:
			return r;
		case 1:
			r[i] ^= m; // clear bit
			break;
		}

		k--;
	}

	return null;
}

function
bignum_int(n)
{
	var u = [];
	u[0] = n;
	return u;
}

function
bignum_copy(u)
{
	var i, v = [];
	for (i = 0; i < u.length; i++)
		v[i] = u[i];
	return v;
}

// remove leading zeroes

function
bignum_norm(u)
{
	while (u.length > 1 && u[u.length - 1] == 0)
		u.pop();
}
