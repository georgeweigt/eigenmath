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

// convert bignum to uint32

function
bignum_uint32(u)
{
	if (u.length == 1)
		return u[0];

	if (u.length == 2 && u[1] < 256)
		return BIGM * u[1] + u[0];

	return null;
}

// convert bignum to int32

function
bignum_smallnum(u)
{
	if (u.length == 1)
		return u[0];

	if (u.length == 2 && u[1] < 128)
		return BIGM * u[1] + u[0];

	return null;
}

function
bignum_issmallnum(u)
{
	return u.length == 1 || (u.length == 2 && u[1] < 128);
}

function
push_bignum(sign, a, b)
{
	// normalize zero

	if (bignum_iszero(a)) {
		sign = 1;
		if (!bignum_equal(b, 1))
			b = bignum_int(1);
	}

	push({sign:sign, a:a, b:b});
}
