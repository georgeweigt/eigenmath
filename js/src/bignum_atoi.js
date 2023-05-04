// 7 decimal digits fits in 24 bits

function
bignum_atoi(s)
{
	var a, b, k;

	a = bignum_int(0);
	k = s.length % 7;

	if (k)
		a[0] = Number(s.substr(0, k));

	if (k == s.length)
		return a;

	b = bignum_int(0);

	while (k < s.length) {
		b[0] = 10000000; // 10^7
		a = bignum_mul(a, b);
		b[0] = Number(s.substr(k, 7));
		a = bignum_add(a, b);
		k += 7;
	}

	return a;
}
