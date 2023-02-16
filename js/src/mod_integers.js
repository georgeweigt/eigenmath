function
mod_integers(p1, p2)
{
	var a, b;

	a = bignum_mod(p1.a, p2.a);
	b = bignum_int(1);

	push_bignum(p1.sign, a, b);
}
