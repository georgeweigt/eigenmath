/* exported bignum_int */
/* exported bignum_add */
/* exported bignum_sub */
/* exported bignum_mul */
/* exported bignum_div */
/* exported bignum_mod */
/* exported bignum_pow */
/* exported bignum_shr */
/* exported bignum_gcd */
/* exported bignum_cmp */
/* exported bignum_root */
/* exported bignum_copy */
/* exported bignum_norm */
/* exported bignum_atoi */
/* exported bignum_itoa */
/* exported bignum_iszero */
/* exported bignum_equal */

const BIGM = 0x1000000; // 24 bits

function
bignum_int(n)
{
	var u = [];

	if (n < BIGM)
		u[0] = n;
	else {
		u[0] = n % BIGM;
		u[1] = Math.floor(n / BIGM);
	}

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

function
bignum_iszero(u)
{
	return bignum_equal(u, 0);
}

function
bignum_equal(u, n)
{
	return u.length == 1 && u[0] == n;
}

function
bignum_odd(u)
{
	return u[0] % 2 == 1;
}

function
bignum_float(u)
{
	var d, i;

	d = 0;

	for (i = u.length - 1; i >= 0; i--)
		d = BIGM * d + u[i];

	if (!isFinite(d))
		stopf("floating point nan or infinity");

	return d;
}
