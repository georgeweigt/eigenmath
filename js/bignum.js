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
/* exported bignum_int */
/* exported bignum_copy */
/* exported bignum_norm */

const BIGM = 0x1000000; // 24 bits

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
