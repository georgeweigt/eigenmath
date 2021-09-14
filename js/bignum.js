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

function
bignum_add(u, v)
{
	var i, nu, nv, nw, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] + v[0];
		if (w[0] <= 0xffffff)
			return w;
	}

	if (nu > nv)
		nw = nu + 1;
	else
		nw = nv + 1;

	for (i = 0; i < nu; i++)
		w[i] = u[i];

	for (i = nu; i < nw; i++)
		w[i] = 0;

	t = 0;

	for (i = 0; i < nv; i++) {
		t += w[i] + v[i];
		w[i] = t & 0xffffff;
		t >>= 24;
	}

	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = t & 0xffffff;
		t >>= 24;
	}

	bignum_norm(w);

	return w;
}

// u is greater than or equal to v

function
bignum_sub(u, v)
{
	var i, nu, nv, nw, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] - v[0];
		if (w[0] <= 0xffffff)
			return w;
	}

	if (nu > nv)
		nw = nu;
	else
		nw = nv;

	for (i = 0; i < nu; i++)
		w[i] = u[i];

	for (i = nu; i < nw; i++)
		w[i] = 0;

	t = 0;

	for (i = 0; i < nv; i++) {
		t += w[i] - v[i];
		w[i] = Math.abs(t) & 0xffffff;
		t >>= 24;
	}

	for (i = nv; i < nw; i++) {
		t += w[i];
		w[i] = Math.abs(t) & 0xffffff;
		t >>= 24;
	}

	bignum_norm(w);

	return w;
}

function
bignum_mul(u, v)
{
	var i, j, nu, nv, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nu == 1 && nv == 1) {
		w[0] = u[0] * v[0];
		if (w[0] <= 0xffffff)
			return w;
	}

	for (i = 0; i < nu; i++)
		w[i] = 0;

	for (j = 0; j < nv; j++) {
		t = 0;
		for (i = 0; i < nu; i++) {
			t += u[i] * v[j] + w[i + j];
			w[i + j] = t & 0xffffff;
			t >>= 24;
		}
		w[i + j] = t & 0xffffff;
	}

	bignum_norm(w);

	return w;
}

// floor(u / v)

function
bignum_div(u, v)
{
	var a, b, i, k, nu, nv, q = [], qhat, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nv == 1 && v[0] == 0)
		stopf("divide by zero");

	if (nu == 1 && nv == 1) {
		q[0] = (u[0] / v[0]) & 0xffffff;
		return q;
	}

	k = nu - nv;

	if (k < 0) {
		q[0] = 0;
		return q; // u < v, return zero
	}

	u = bignum_copy(u);

	b = v[nv - 1];

	do {
		q[k] = 0;
		while (nu >= nv + k) {
			// estimate partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = (a << 24) | u[nu - 2];
			if (a < b)
				break;
			qhat = (a / (b + 1)) & 0xffffff; // mask to truncate
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += qhat * v[i];
				w[i] = t & 0xffffff;
				t >>= 24;
			}
			w[nv] = t & 0xffffff;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += u[i] - w[i - k];
				u[i] = Math.abs(t) & 0xffffff;
				t >>= 24;
			}
			if (t != 0) {
				// u is negative, restore u and break
				t = 0;
				for (i = k; i < nu; i++) {
					t += u[i] + w[i - k];
					u[i] = t & 0xffffff;
					t >>= 24;
				}
				break;
			}
			q[k] += qhat;
			bignum_norm(u);
			nu = u.length;
		}
	} while (--k >= 0);

	q = bignum_norm(q);

	return q;
}

// u mod v

function
bignum_mod(u, v)
{
	var a, b, i, k, nu, nv, qhat, t, w = [];

	nu = u.length;
	nv = v.length;

	if (nv == 1 && v[0] == 0)
		stopf("divide by zero"); // v = 0

	u = bignum_copy(u);

	k = nu - nv;

	if (k < 0)
		return u; // u < v

	b = v[nv - 1];

	do {
		while (nu >= nv + k) {
			// estimate partial quotient
			a = u[nu - 1];
			if (nu > nv + k)
				a = (a << 24) + u[nu - 2];
			if (a < b)
				break;
			qhat = (a / (b + 1)) & 0xffffff; // mask to truncate
			if (qhat == 0)
				qhat = 1;
			// w = qhat * v
			t = 0;
			for (i = 0; i < nv; i++) {
				t += qhat * v[i];
				w[i] = t & 0xffffff;
				t >>= 24;
			}
			w[nv] = t & 0xffffff;
			// u = u - w
			t = 0;
			for (i = k; i < nu; i++) {
				t += u[i] - w[i - k];
				u[i] = Math.abs(t) & 0xffffff;
				t >>= 24;
			}
			if (t != 0) {
				// u is negative, restore u and break
				t = 0;
				for (i = k; i < nu; i++) {
					t += u[i] + w[i - k];
					u[i] = t & 0xffffff;
					t >>= 24;
				}
				break;
			}
			u = bignum_norm(u);
			nu = u.length;
		}
	} while (--k >= 0);

	return u;
}

// u ^ v

function
bignum_pow(u, v)
{
	var w = [];

	u = bignum_copy(u);
	v = bignum_copy(v);

	w[0] = 1;

	for (;;) {
		if (v[0] & 1)
			w = bignum_mul(w, u);
		bignum_shr(v);
		if (v.length == 1 && v[0] == 0)
			break;
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
	var i, j, k, m, r = [], t = [];

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
