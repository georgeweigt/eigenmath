/*
exported bignum_add
exported bignum_sub
exported bignum_mul
exported bignum_div
exported bignum_mod
*/

function
bignum_add(u, v)
{
	return u + v;
}

function
bignum_sub(u, v)
{
	return u - v;
}

function
bignum_mul(u, v)
{
	return u * v;
}

function
bignum_div(u, v)
{
	return Math.floor(u / v);
}

function
bignum_mod(u, v)
{
	return u % v;
}

function
bignum_int(n)
{
	return n;
}

function
bignum_atoi(s)
{
	return parseInt(s);
}

function
bignum_itoa(u)
{
	return Math.abs(u).toFixed(0);
}
