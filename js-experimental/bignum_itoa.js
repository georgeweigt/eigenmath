function
bignum_itoa(u)
{
	var d, r, s;

	if (u.length == 1)
		return String(u[0]);

	d = bignum_int(10000000); // d = 10^7

	s = "";

	while (u.length > 1 || u[0] >= 10000000) {

		r = bignum_mod(u, d);
		u = bignum_div(u, d);

		s = String(r[0]).concat(s);

		while (s.length % 7)
			s = "0".concat(s); // add leading zeroes
	}

	s = String(u[0]).concat(s);

	return s;
}
