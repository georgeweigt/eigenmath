function
pop_double()
{
	var a, b, p;

	p = pop();

	if (isrational(p)) {
		a = bignum_float(p.a);
		b = bignum_float(p.b);
		if (isnegativenumber(p))
			a = -a;
		return a / b;
	}

	if (isdouble(p))
		return p.d;

	stopf("number expected");
}
