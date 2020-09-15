function
simplify_polar_term(p)
{
	var d, n;

	if (car(p) != symbol(MULTIPLY))
		return 0;

	// exp(i pi) -> -1

	if (length(p) == 3 && isimaginaryunit(cadr(p)) && caddr(p) == symbol(PI)) {
		push_integer(-1);
		return 1;
	}

	if (length(p) != 4 || !isnum(cadr(p)) || !isimaginaryunit(caddr(p)) || cadddr(p) != symbol(PI))
		return 0;

	p = cadr(p); // coeff

	if (isdouble(p)) {
		d = p.d;
		if (Math.floor(d) == d) {
			if (d % 2 == 0)
				push_double(1.0);
			else
				push_double(-1.0);
			return 1;
		}
		if (Math.floor(d) + 0.5 == d) {
			n = (d / 0.5) % 4;
			if (n == 1 || n == -3) {
				push_symbol(MULTIPLY);
				push_double(1.0);
				push(imaginaryunit);
				list(3);
			} else {
				push_symbol(MULTIPLY);
				push_double(-1.0);
				push(imaginaryunit);
				list(3);
			}
			return 1;
		}
		return 0;
	}

	if (p.b == 1) {
		if (Math.abs(p.a) % 2 == 0)
			push_integer(1);
		else
			push_integer(-1);
		return 1;
	}

	if (p.b == 2) {
		n = Math.abs(p.a) % 4;
		if ((n == 1 && p.a > 0) || (n == 3 && p.a < 0))
			push(imaginaryunit);
		else {
			push_symbol(MULTIPLY);
			push_integer(-1);
			push(imaginaryunit);
			list(3);
		}
		return 1;
	}

	return 0;
}
