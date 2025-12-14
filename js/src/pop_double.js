function
pop_double()
{
	var a, b, d, p;
	p = pop();
	if (!isnum(p))
		stopf("argument is not a number");
	if (isdouble(p))
		d = p.d;
	else {
		a = bignum_float(p.a);
		b = bignum_float(p.b);
		d = a / b;
		if (isnegativenumber(p))
			d = -d;
	}
	return d;
}
