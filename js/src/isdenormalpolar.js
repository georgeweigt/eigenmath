function
isdenormalpolar(p)
{
	if (car(p) == symbol(ADD)) {
		p = cdr(p);
		while (iscons(p)) {
			if (isdenormalpolarterm(car(p)))
				return 1;
			p = cdr(p);
		}
		return 0;
	}

	return isdenormalpolarterm(p);
}

function
isdenormalpolarterm(p)
{
	var t;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	if (lengthf(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI))
		return 1; // exp(i pi)

	if (lengthf(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // p = coeff of term

	if (isdouble(p))
		return p.d < 0 || p.d >= 0.5;

	push(p);
	push_rational(1, 2);
	t = cmpfunc();

	if (t >= 0)
		return 1; // p >= 1/2

	push(p);
	push_integer(0);
	t = cmpfunc();

	if (t < 0)
		return 1; // p < 0

	return 0;
}
